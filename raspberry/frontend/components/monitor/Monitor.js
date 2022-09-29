/*
============================================================================
== SERVICES DEPENDENCIES ==                                               ==
============================================================================ */
Vue.mixin({
    // Creates instances of desired services.
    // This behaviour is for to implement a kind of 'dependency injection'.
    //   To inject a service in a component, attach the desired service placed 
    //   on $root to some property in the 'beforeCreate' component event.
    beforeCreate: function() {

        // Monitor service
        this.$root.$monitorService = 
            this.$root.$monitorService || new MonitorService();
        
        // Socket service
        this.$root.$socket = 
            this.$root.$socket || new SocketService();

        // Session service
        this.$root.$sessionService = 
            this.$root.$sessionService || new SessionService();
    }
});

/*
======================================================================================
== MONITOR COMPONENT ==                                                             ==
== Defines elements to monitoring patient, configure modes parameters, alarms, etc. ==
== It starts getting and using the setup configuration from navigator session.      ==
====================================================================================== */
Vue.component('monitor', {

    // Componente data
    data: function() {
        return {
            patient: '',
            modes: [],
            sensors: [],
            curves: [],
            graphs: [],
            
            messages: [],
            alarms: [],
            outputs: [],
            hourmeter: '',
            
            mode: '',
            powerIcon: './images/indicador_tomada.png',
            batteryIcon: './images/indicador_bateria_100_nobg.png',
            alertFunctions: {
                'noPower': {
                    'onActivate': this.getChangePowerIconFunc('indicador_tomada_off.png'),
                    'callback': this.getChangePowerIconFunc('indicador_tomada.png')
                },
                'battery75': {
                    'onActivate': this.getChangeBatteryIconFunc('indicador_bateria_75_nobg.png'),
                    'callback': this.getChangeBatteryIconFunc('indicador_bateria_100_nobg.png')
                },
                'battery50': {
                    'onActivate': this.getChangeBatteryIconFunc('indicador_bateria_50_nobg.png'),
                    'callback': this.getChangeBatteryIconFunc('indicador_bateria_100_nobg.png')
                },
                'battery25': {
                    'onActivate': this.getChangeBatteryIconFunc('indicador_bateria_25_nobg.png'),
                    'callback': this.getChangeBatteryIconFunc('indicador_bateria_100_nobg.png')
                },
                'backupOn': {
                    'onActivate': this.showModeBK(true),
                    'callback': this.showModeBK(false),
                },
            },
            parameters: [],
            graphShapeParameter: null,

            blockEnable: false,
            showConfirmationWindow: false,
            showAlarmMenu: false,
            showSubMenu: true,
            showModeMenu: false,
            resetModeMenuParams: false,
            showPatientInfo: false,
            showMainSideMenu: true,
            showAlarmHistory: false,
            ventilatorStatus: 'STAND BY',
            confirmationMessage: '',
            confirmationCallback: () => {},
            triggerParamsIds: ['sensInsP', 'sensInpF', 'sensExpF'],
            backupParamsIds: ['bkMode', 'bkTIns', 'bkFreq', 'bkVTidal', 'bkPcon'],
            pressureParams: ['pCon', 'pSupport', 'pInsLimit'],
            calibrationParams: { "PAr": 0, "PO2": 0 },
            selectedFooterParam: {},
            alertsTimeout: [],
            sensorsTimeout: [],
            bkModeActivated: false,

            breathingPhase: '',
            customGraph: {setted: false},
            customGraphHistory: [],
            customGraphOptions: [],
            customGraphValue: [],
            customGraphValueOverwrite: [],
            customGraphSettings: {
                x: { axis: "", unit: "", range: [0, 0] },
                y: { axis: "", unit: "", range: [0, 0] }
            },

            sensorGroups: {
                "P Plateau": [
                    {
                        key: "P Pico",
                        value: {}
                    },
                    {
                        key: "P Media",
                        value: {}
                    },
                ],
                "V Tidal": [
                    {
                        key: "V Minuto",
                        value: {}
                    },
                ],
                "PEEP": [
                    {
                        key: "FIO2",
                        value: {}
                    },
                ],
                "Freq": [
                    {
                        key: "T Ins",
                        value: {}
                    },
                ],
                "C Est": [
                    {
                        key: "C Din",
                        value: {}
                    },
                ],
            },
            indicateBlink: false,
        }
    },

    // Service injections
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
        this.$socket = this.$root.$socket;
    },

    // Component created event
    created: function () {

        // Monitor startup
        this.startup();

        // Listening components events
        this.listenRequiredEvents();
    },

    // HTML template
    template: `
        <div>
            <!-- Full screen div to block screen when blockEnable = true -->
            <div id="blockDiv" v-bind:class="{ '-blink': indicateBlink }" v-show="blockEnable" v-on:click="indicateBlock()"></div>
            
            <!-- Monitor header -->
            <div class="header">

                <div class="logo">
                    <img src="./images/logo_exigido-2.png" alt="">
                </div>

                <!-- Alerts -->
                <div class="alerts" v-if="!showConfirmationWindow" v-on:click="silenceAlarms()">
                    <alert-pane v-bind:messages="messages"></alert-pane>
                </div>


                <!-- Confirmation Alerts -->
                <div class="alerts" v-if="showConfirmationWindow">
                    <confirm-window v-bind:message="confirmationMessage" v-bind:confirmationCallback="confirmationCallback" v-on:closeWindow="closeConfirmationWindow"></confirm-window>
                </div>

                <div class="mode-display">
                    <span class="selected">{{ mode }} {{ bkModeActivated ? 'BK' : '' }}</span>
                    
                </div>

                <!-- Clock and power -->
                <clock-pane v-bind:hourmeter="hourmeter" v-bind:powerIcon="powerIcon" v-bind:batteryIcon="batteryIcon"></clock-pane>
            </div>

            <!-- Monitor main screen -->
            <div class="main">

                <!-- Sensors monitoring -->
                <div class="monitoring-1">
                    <sensor-item v-for="sensor in sensors" v-bind:sensor="sensor" v-bind:sensorGroup="sensorGroups[sensor.name]" v-bind:mode="mode"
                        v-if="sensor.name == 'Freq' || sensor.name == 'P Plateau' || sensor.name == 'V Tidal' || sensor.name == 'PEEP' || sensor.name == 'C Est'">
                    </sensor-item>
                </div>

                <!-- Graphs monitoring -->
                <div class="graphs" v-if="graphs[0]">
                    <graph-frame v-bind:id="graphs[1].id" v-bind:title="graphs[1].title" v-bind:ref="graphs[1].id"
                        v-bind:xunit="graphs[1].xunit" v-bind:yunit="graphs[1].yunit"
                        v-bind:axisPosition="graphs[1].axisPosition" v-bind:color="graphs[1].color"
                        v-bind:height="graphs[1].height" v-bind:width="graphs[1].width" v-bind:margin="graphs[1].margin"
                        v-bind:dataset="graphs[1].dataset" v-bind:yaxis="graphs[1].axisY" v-bind:xaxis="graphs[1].axisX">
                    </graph-frame>
                    <graph-frame v-bind:id="graphs[2].id" v-bind:title="graphs[2].title" v-bind:ref="graphs[2].id"
                        v-bind:xunit="graphs[2].xunit" v-bind:yunit="graphs[2].yunit" 
                        v-bind:axisPosition="graphs[2].axisPosition" v-bind:color="graphs[2].color"
                        v-bind:height="graphs[2].height" v-bind:width="graphs[2].width" v-bind:margin="graphs[2].margin"
                        v-bind:dataset="graphs[2].dataset" v-bind:yaxis="graphs[2].axisY" v-bind:xaxis="graphs[2].axisX">
                    </graph-frame>
                    <graph-frame v-bind:id="graphs[0].id" v-bind:title="graphs[0].title" v-bind:ref="graphs[0].id"
                        v-bind:xunit="graphs[0].xunit" v-bind:yunit="graphs[0].yunit" 
                        v-bind:axisPosition="graphs[0].axisPosition" v-bind:color="graphs[0].color"
                        v-bind:height="graphs[0].height" v-bind:width="graphs[0].width" v-bind:margin="graphs[0].margin"
                        v-bind:dataset="graphs[0].dataset" v-bind:yaxis="graphs[0].axisY" v-bind:xaxis="graphs[0].axisX">
                    </graph-frame>
                    <div v-if="!this.customGraph.setted" class="graph-settings">
                        <p class="title">Gráfico Personalizado</p>

                        <div class="options">
                            <div class="group y">
                                <p>Eixo Y</p>
                                <div class="option" v-for="option in customGraphOptions">
                                    <label v-bind:key="option.value">
                                        <input type="radio" v-model="customGraphSettings.y" v-bind:value="option.value">
                                        <p>{{option.text}}</p>
                                    </label>
                                </div>
                            </div>
                            <div class="group x">
                                <p>Eixo X</p>
                                <div class="option" v-for="option in customGraphOptions">
                                    <label v-bind:key="option.value">
                                        <input type="radio" v-model="customGraphSettings.x" v-bind:value="option.value">
                                        <p>{{option.text}}</p>
                                    </label>
                                </div>
                            </div>
                        </div>
                        
                        <div class="confirm">
                            <button v-if="this.customGraphSettings.y.axis && this.customGraphSettings.x.axis
                                && this.customGraphSettings.y.axis != this.customGraphSettings.x.axis" class="btn -outline" v-on:click="setCustomGraph()">
                                    Aceitar
                                </button>
                                <button v-if="!this.customGraphSettings.y.axis || !this.customGraphSettings.x.axis
                                || this.customGraphSettings.y.axis == this.customGraphSettings.x.axis" class="btn -outline -disabled">
                                    Selecione os Eixos
                                </button>
                        </div>
                    </div>
                    <graph-frame v-if="this.customGraph.setted" v-bind:id="graphs[3].id" v-bind:ref="graphs[3].id"
                        v-bind:title="graphs[3].title" v-bind:xunit="graphs[3].xunit" v-bind:yunit="graphs[3].yunit" 
                        v-bind:axisPosition="graphs[3].axisPosition" v-bind:color="graphs[3].color"
                        v-bind:height="graphs[3].height" v-bind:width="graphs[3].width" v-bind:margin="graphs[3].margin"
                        v-bind:dataset="graphs[3].dataset" v-bind:yaxis="graphs[3].axisY" v-bind:xaxis="graphs[3].axisX">
                    </graph-frame>
                    <button v-if="this.customGraph.setted" class="btn -outline config" v-on:click="resetCustomGraph()">Configurar</button>
                </div>

                <!-- Side menu -->
                <div class="sidemenu">

                    <div class="item" v-on:click="this.openModeMenu" v-bind:class="[showModeMenu ? '-active' : '']" v-if="showSubMenu">
                        <span>Modo de Ventilação</span>
                    </div>

                    <!-- Mode select -->
                    <mode-select-sidebar
                        v-bind:ref="'modeMenu'"
                        v-bind:showModeMenu="this.showModeMenu" 
                        v-bind:modes="modes" 
                        v-bind:triggerParamsIds="this.triggerParamsIds"
                        v-bind:backupParamsIds="this.backupParamsIds"
                    />
                    
                    <div class="main-menu" v-show="showMainSideMenu">

                        <!-- Alarms -->
                        <div class="item" v-on:click="openAlarm"  v-bind:class="[showAlarmMenu ? '-active' : '']">
                            <span>Alarmes</span>
                        </div>
                        
                        <div class="item" v-on:click="openAlarmHistory">
                            <span>Log</span>
                        </div>

                        <div class="item" v-on:click="openPatientInfo" v-bind:class="[showPatientInfo ? '-active' : '']">
                            <span>Info. Paciente</span>
                        </div>

                        
                        <div class="item" v-if="ventilatorStatus === 'EM OPERAÇÃO'" v-on:click="stopController">
                            <span>Parar</span>
                        </div>

                        <div class="item" v-if="ventilatorStatus !== 'EM OPERAÇÃO'" v-on:click="startController()">
                            <span>Iniciar</span>
                        </div>
                        
                        <div class="item turnoff" v-if="ventilatorStatus !== 'EM OPERAÇÃO'" v-on:click="turnOff">
                            <span>Desligar</span>
                        </div>

                        <!-- Alarms configuration -->                    
                        <div class="alarms" v-bind:class="[showAlarmMenu ? '-show' : '-hide']">
                            <alarms-config v-bind:alarms="alarms" v-bind:ref="'alarmConfig'"></alarms-config>
                        </div>

                        <!-- Patient info panel -->
                        <div class="patient-info" v-bind:class="[showPatientInfo ? '-show' : '-hide']">
                            <div style="display: flex;justify-content: space-between; align-items: flex-end;">
                                <h2 class="main-title">Informações do Paciente</h2>
                                <span style="font-size: 1.7rem; display: flex; justify-content: flex-end;font-family:sans-serif, Arial, Helvetica;">
                                    {{this.calibrationParams["PAr"]}} , {{this.calibrationParams["PO2"]}}
                                </span>
                            </div>
                            <patient-data-view v-bind:patient-data="patient" v-bind:mode="mode"/>
                            <div style="display: flex; justify-content: flex-end;">
                                <button class="btn -outline" v-bind:class="[ventilatorStatus === 'EM OPERAÇÃO' ? '-disabled' : '']" 
                                    style="margin-left: 2.75rem;" v-on:click="goToSetup()">
                                    Trocar Paciente
                                </button>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Footer region -->
            <div class="footer">
                <div class="programables">
                    <!-- The set of mode parameters to configure in real time -->
                    <param-input 
                        v-for="parameter in parameters" 
                        v-if="!triggerParamsIds.includes(parameter.id) && !backupParamsIds.includes(parameter.id)"
                        v-bind:parameter="parameter" 
                        v-bind:selectedFooterParam="selectedFooterParam">
                    </param-input>

                    <div class="menu">
                        <div class="menu-icon" v-on:click="openMenu"></div>
                        <div v-bind:class="[ventilatorStatus === 'STAND BY' ? 'menu-standby' :
                                            ventilatorStatus === 'PAUSA' ? 'menu-pause' : 
                                            ventilatorStatus === 'EM OPERAÇÃO' ? 'menu-operating' : '']">
                            <span>{{this.ventilatorStatus}}</span>
                            <div class='status-line'></div>
                        </div>
                    </div>

                </div>
            </div>
            <!-- Alarms History -->                    
            <div class="alarm-overlay" v-if="showAlarmHistory">
                <alarm-history v-model="showAlarmHistory"></alarm-history>
            </div>
        </div>
    `,

    // Component methods
    methods: {
        blockScreen() {
            this.blockEnable = !this.blockEnable;
        },
        indicateBlock() {
            if(!this.indicateBlink){
                this.indicateBlink = true;
                setTimeout(() => {
                    this.indicateBlink = false;
                }, 500);
            }
        },
        silenceAlarms() {
            if(!this.showConfirmationWindow && this.messages.length != 0) {
                this.$monitorService.sendSilence(() => {});
            }
        },
        showModeBK(value) {
            return () => {
                this.bkModeActivated = value;
            }
        },
        startup() {

            // Get all mode settings from API
            this.$monitorService.getSettings(response => {

                // Populates modes
                this.modes = response.modes;
                
                this.hourmeter = this.$sessionService.get('hourmeter');
                
                // Gets setup configuration
                let setupConfiguration = 
                    this.$sessionService.get('setupConfiguration');

                // Gets mode, alarms and hourmeter configs
                let setupMode = setupConfiguration.mode;
                let setupAlarms = setupConfiguration.alarms;

                // Populates modes-select input
                this.createParametersPerMode(this.modes);

                // Applies setup mode and alarms
                this.populateAlarms(setupAlarms);
                this.changeModeHandler(setupMode);

                // Creates sensors
                this.createSensors(response.output);

                // Creates graphs
                this.createGraphs(response.graphics);

                // Create Personalized graph
                let setupCustomGraph = this.$sessionService.get('customGraph');
                if(setupCustomGraph) {
                    Object.assign(this.customGraph, setupCustomGraph.customGraph);
                    this.customGraph.setted = true;
                    this.graphs[3] = this.customGraph;
                }

                // Create options for custom graph
                response.graphics.forEach(g => {
                    this.customGraphOptions.push({text: g.name, value: {axis: g.name, unit: g.yunit, range: g.yrange}})
                });

                // Registers socket events
                this.registerSocketEvents();

                // Alternating current message
                this.alternateDisplayMessages();

                // Remap graphs to trigger Vue update
                this.drawGraphics();

                // Update sensors received data
                this.updateSensors();
                
            });

            // If configured, runs Socket IO simulations
            if(appSettings.simulateSocketIo)
                this.simulateSockectIo();
        },
        listenRequiredEvents() {

            // Event when mode is changing
            this.$on('changeMode', mode => {
                this.changeModeHandler(mode);
            });

            // Event when footer param is selected
            this.$on('selection', parameter => {
                this.showPatientInfo = false;
                this.showAlarmMenu = false;
                if(this.showModeMenu)
                    this.openModeMenu();

                this.selectedFooterParam = parameter;
                if(!this.blockEnable)
                    this.blockScreen();
            });

            // Event to apply a parameter, sending it value to API
            this.$on('applyParameter', parameter => {
                if(this.mode.startsWith('SIMV')) {
                    let paramsFake = ['Freq', 'vTidal', 'pCon'];
                    if(!paramsFake.includes(parameter.id))
                        this.$monitorService.sendParameter('PSV', parameter);
                } else {
                    this.$monitorService.sendParameter(this.mode, parameter);
                }

                this.updateSessionData();
                this.selectedFooterParam = {};
                this.blockScreen();

                // Update to ModeMenu
                this.$refs['modeMenu'].updateValue(this.mode, parameter);

                // Update to Alarms
                this.updateAlarmValues(parameter);

                // Graphs limits update
                this.updateGraphLimits(this.mode, this.parameters)
            });

            this.$on('resetParameter', () => {
                this.selectedFooterParam = {};
                this.blockScreen();
            });
        },
        updateAlarmValues(parameter) {

            if(this.pressureParams.includes(parameter.id)) {
                this.$refs['alarmConfig'].updateBoundaries([
                    {id: 'pInsMax', value: parameter.value},
                    {id: 'pInsMin', value: parameter.value}
                ]);
            }

        },
        resetGraphLimits() {

            this.graphs.forEach((graph, index)=> {
                // Skip custom graph
                if(index < 3) {
                    graph.axisY[1] = graph.originalAxisYMax;
                    this.$refs[graph.id].updateLimits();
                }
            });

        },
        updateGraphLimits(mode, parameters) {

            this.resetGraphLimits();
            const limitsByModes = [
                {mode: 'VCV', parameters: [{id: 'vTidal', graphId: 2, modifier: 1.5, round: 5}, {id: 'pInsLimit', graphId: 1, modifier: 1.25, round: 5}]},
                {mode: 'PCV', parameters: [{id: 'pCon', graphId: 1, modifier: 1.5, round: 5}]},
                {mode: 'PSV', parameters: [{id: 'pSupport', graphId: 1, modifier: 1.5, round: 5}]},
                {mode: 'SIMV(VC) + PS', parameters: [{id: 'vTidal', graphId: 2, modifier: 1.5, round: 5}, {id: 'pInsLimit', graphId: 1, modifier: 1.25, round: 5}]},
                {mode: 'SIMV(PC) + PS', parameters: [{id: 'pCon', graphId: 1, modifier: 1.5, round: 5}]},
            ];
            let limits = limitsByModes.find(m => m.mode === mode);
            if(limits) {
                limits.parameters.forEach(limit => {
                    parameter = parameters.find(p => p.id === limit.id);
                    if(parameter) {
                        let value = Math.round((parameter.value * limit.modifier) / limit.round) * limit.round;
                        this.graphs[limit.graphId].axisY[1] = value;
                        this.$refs[this.graphs[limit.graphId].id].updateLimits();
                    }
                });
            }

            if(this.customGraph.setted) this.$refs[this.graphs[3].id].updateLimits();
            
        },
        setCustomGraph() {
            Object.assign(this.customGraph, {
                xaxis: this.customGraphSettings.x.axis,
                yaxis: this.customGraphSettings.y.axis,
                title: this.customGraphSettings.y.axis + ' por ' + this.customGraphSettings.x.axis,
                xunit: this.customGraphSettings.x.unit,
                yunit: this.customGraphSettings.y.unit,
                axisX: this.customGraphSettings.x.range,
                axisY: this.customGraphSettings.y.range
            });

            this.customGraph.setted = true;
            this.graphs[3] = this.customGraph;
            // this.$sessionService.set('customGraph', {customGraph: this.customGraph});
        },
        resetCustomGraph() {
            this.customGraph.setted = false;
            this.customGraphHistory = [];
            this.customGraphValueOverwrite = [];
            this.customGraphValue = [];
            this.graphs[3] = this.customGraph;
        },
        createParametersPerMode(modes) {

            // Iterating by modes
            modes.forEach(mode => {

                // Sets graph shape options
                mode.graphShape = mode.configuration
                    .find(cfg => cfg.id == 'shape')

                // Sets mode parameters
                mode.parameters = mode.configuration
                    .filter(cfg => cfg.id != 'shape');
            });

        },
        populateAlarms(alarms) {

            // Sets configured alarms
            this.alarms = alarms;
        },
        changeModeHandler(mode) {

            // Clears current parameters
            this.mode = '';
            this.graphShapeParameter = null;
            this.parameters = [];

            // Sets parameters from changed mode
            setTimeout(() => {
                
                // Mode applied data properties
                if (mode.patient)
                    this.patient = mode.patient;
                this.mode = mode.mode;
                this.graphShapeParameter = mode.graphShape;
                this.parameters = mode.parameters;

                // Updates graphs limits
                this.updateGraphLimits(mode.mode, mode.parameters);

                // Update alarm values
                mode.parameters.forEach(parameter => {
                    this.updateAlarmValues(parameter);
                });

                // Updates session setupConfiguration
                this.updateSessionData();
                
            }, 75);

            // Notifies mode load to mode-select component
            this.$root.$emit('modeLoaded', mode);
           
        },
        updateSessionData() {

            // Applies to session storage updated configuration
            this.$sessionService.set('setupConfiguration', { 

                // Defines mode config
                mode: { 
                    patient: this.patient,
                    mode: this.mode,
                    graphShape: this.graphShapeParameter,
                    parameters: this.parameters
                },

                // Defines alarms config
                alarms: this.alarms
            });

        },
        createSensors(sensors) {

            // Creates each sensor to monitoring
            sensors.forEach(sensor => {

                // Defines sensor model
                var sensorModel = {

                    // Sensor data properties
                    name: sensor.name,
                    value: 0,
                    min: sensor.range[0],
                    max: sensor.range[1],
                    unit: sensor.unit,

                    // Properties to verify sensor timeout
                    lastUpdate: new Date().getTime(),
                    timeOut: this.sensorTimeoutVerifier(sensor),

                    //Property to get some associated alert
                    alert: undefined

                };

                // Adds sensor model to monitoring
                this.outputs.push(sensorModel);

            });
        },
        createGraphs(graphics) {
            // For now, we have 4 graphs: [0,1,2,3]
            // Starting to configure at first, index=0
            var index = 0;

            // Width, color and axis position configs
            var widths = [610, 610, 610, 610]
            var colors = ['lime', 'fuchsia', 'cyan', 'yellow'];
            var axisPositions = ['center', 'bottom', 'bottom', 'bottom'];

            // Configuring each graphic
            graphics.forEach(graph => {
                // Defines graph model
                var graphModel = {
                    title: graph.name,
                    xunit: graph.xunit,
                    yunit: graph.yunit,
                    id: `graph${index}`,
                    timestamp: 0,
                    height: 285,
                    width: widths[index],
                    axisY: graph.yrange,
                    originalAxisYMax: graph.yrange[1],
                    axisX: graph.xrange,
                    margin: { top: 8, right: 40, bottom: 8, left: 30 },
                    color: colors[index],
                    axisPosition: axisPositions[index]
                }

                // Adds graph to monitoring and triggers drawHandler
                this.graphs.push(graphModel);
            
                // Goes to the next one
                index++;

            });

            // Create custom graph
            this.customGraph = {
                id: `graph3`,
                title: '',
                timestamp: 0,
                height: 270,
                width: widths[3],
                margin: { top: 8, right: 40, bottom: 20, left: 30 },
                color: colors[3],
                axisPosition: axisPositions[3]
            };

            // add as fourth graph of graphs
            this.graphs.push(this.customGraph);

        },
        registerSocketEvents() {

            // Registering socket events to listen
            this.$socket.registerEvent('report', this.reportHandler);
            this.$socket.registerEvent('alert', this.alertHandler);
            this.$socket.registerEvent('draw', this.drawHandler);
            this.$socket.registerEvent('update hourmeter', this.hourmeterHandler);

        },
        reportHandler(message) {
            
            // Receiving updated sensors data to show while on ventilation
            if(this.ventilatorStatus === 'EM OPERAÇÃO') {
                message.output.forEach(sensorUpdate => {
    
                    // Gets sensor to update
                    let sensor = this.outputs
                        .find(sensor => sensor.name == sensorUpdate.name);
    
                    // Verify whether has some alert for this sensor
                    let alert = this.getSensorAlert(sensor, sensorUpdate);
    
                    // Applies updated data
                    sensor.min = sensorUpdate.range[0];
                    sensor.max = sensorUpdate.range[1];
                    sensor.value = sensorUpdate.value;
                    sensor.lastUpdate = new Date().getTime();
                    sensor.alert = alert;
                });
            }

            // get pressures
            this.calibrationParams["PAr"] = message.pressures.Ar;
            this.calibrationParams["PO2"] = message.pressures.O2;


            // get pressures
            message.pressures.forEach(pressure => this.calibrationParams[pressure.id] = pressure.value);

        },
        alertHandler(message) {

            // Verifying if we have alerts
            if(message.alerts.length) {

                // Receiving current alerts to show
                let alerts = message.alerts;
		
                // Getting alerts to add
                let alertsToAdd = 
                    alerts.filter(a => this.messages.every(m => m.id != a.id))

                // Configuring properties to verify alarm timeout
                alertsToAdd.forEach(alert => {
                    alert.receivedTime = new Date().getTime();
                    alert.timeOut = this.alarmTimeoutVerifier(alert)
                    alert.boundId = alert.bound_id;
                    delete alert.bound_id;

                    // execute associated alert function when alert is activated
                    // and define callback function.
                    if (alert.id in this.alertFunctions) {
                        this.alertFunctions[alert.id].onActivate();
                        alert.callback = this.alertFunctions[alert.id].callback;
                    }
                });

                // Adding new alerts
                this.messages = this.messages.concat(alertsToAdd);
                
                // Updating existent alerts
                this.messages
                    .filter(m => alerts.some(alert => alert.id == m.id))
                    .forEach(alert => {
                    		alert.receivedTime = new Date().getTime();
                            alert.value = alerts.find(newAlert => newAlert.id == alert.id).value
                    	});
            }
        },
        drawHandler(message) {

            const frequencyFilter = 1;
            var alreadyOverwritten = new Set();
            var newPoints = Array.from({length: frequencyFilter}, () => ({x: 0, y: 0}));

            if(this.ventilatorStatus === 'EM OPERAÇÃO') {
                // Receiving graphics data to draw
                message.graphics.forEach((graphData, id) => {
                    // Gets graph to update
                    let graphToUpdate = this.graphs
                        .find(graph => graph.title == graphData.name);
    
                    // Sets received updated data
                    graphToUpdate.timestamp = graphData.timestamp;
    
                    let overwrite = false;
                    if (!alreadyOverwritten.has(graphToUpdate.id)){
                        overwrite = true;
                        alreadyOverwritten.add(graphToUpdate.id);
                    }
    
                    // Trigger event to delegate graph component to draw graphic
                    this.$emit(`draw${graphToUpdate.id}`, graphData.value, overwrite);
    
                    // Update values of custom graph using values from the overwrite graph
                    if(this.customGraph.setted && overwrite) {
    
                        let axis = '';
                        if(this.customGraph.xaxis === graphToUpdate.title) axis = 'x';
                        if(this.customGraph.yaxis === graphToUpdate.title) axis = 'y';
    
                        if(axis) {
                            let value = graphData.value;
    
                            for(let i = 0 ; i < frequencyFilter ; i++) {
                                let v = value[value.length - (frequencyFilter - i)];
                                let pos = newPoints.length - (frequencyFilter - i);
    
    
                                Object.assign(newPoints[pos], {[axis]: v.y});
                            }
                        }
                    }
                });
    
                if(this.customGraph.setted) {
                    if((this.breathingPhase === "Exp" && message.phase === "Ins") || message.phase === '') {
                        // console.log("Cycle completed!");
                        this.customGraphValueOverwrite = this.customGraphValue.slice()
                        this.customGraphValueOverwrite.push(...newPoints);
                        this.customGraphValue = [];
                    }
    
                    this.customGraphValue.push(...newPoints);
                    let dataset = this.customGraphValueOverwrite.concat(...this.customGraphValue);
                    
                    this.breathingPhase = message.phase;
                    this.$emit(`drawgraph3`, dataset, true);
                    this.customGraphValueOverwrite.splice(0, frequencyFilter)
                    // this.$emit(`drawgraph3`, this.customGraphValueOverwrite, false);
                }
            }
        },
        hourmeterHandler(message) {
            this.hourmeter = message;
        },
        getSensorAlert(sensor, sensorUpdate) {

            // Gets alert from messages
            let fromMessages = this.messages
                .find(m => m.boundId == sensorUpdate.id);
            
            // Gets alert from limits
            // let fromLimits = 
            //     (sensorUpdate.value < sensor.min || sensorUpdate.value > sensor.max)
            //         ? { value: sensorUpdate.value }
            //         : undefined;

            // Returns some alert whether will find any
            return fromMessages;

        },
        sensorTimeoutVerifier(sensor) {
            return setInterval(() => {

                // Getting current moment and delayLimit
                let timeNow = new Date().getTime();
                let delayLimit = timeNow - 1000;

                // Getting sensor to verify
                let sensorToVerify = this.sensors.find(s => s.name == sensor.name);

                // Verifying whether sensor has reached timeout. 
                // If has, apply "---" symbol to indicate timeout to monitor
                if(sensorToVerify.lastUpdate < delayLimit) {
                    sensorToVerify.value = " --- ";
                    sensorToVerify.alert = undefined;
                }

            }, 2000);
        },
        alarmTimeoutVerifier(alert) {
            return setInterval(() => {

                // Getting current moment and delayLimit
                let timeNow = new Date().getTime();
                let delayLimit = timeNow - 5500;

                // Getting alert to verify
                let alertToVerify = this.messages.find(m => m.id == alert.id);
                
                // Verifying whether sensor has reached timeout. 
                // If has, remove alert, clear its interval verifier and execute callback
                // function if defined.
                if(alertToVerify.receivedTime < delayLimit) {
                    this.messages = this.messages.filter(m => m.id != alert.id);
                    setTimeout(() => clearInterval(alert.timeOut), 500);
                    
                    if (alert.callback) {
                        alert.callback();
                    }
                }

            },6250);
        },
        getChangePowerIconFunc(icon) {
            return () => {
                this.powerIcon = `./images/${icon}`;
            }
        },
        getChangeBatteryIconFunc(icon) {
            return () => {
                this.batteryIcon = `./images/${icon}`;
            }
        },
        drawGraphics() {
            setInterval(() => 
                this.graphs = this.graphs.map(g => g)
            , 75);
        },
        updateSensors() {
            setInterval(() => {
                for (const key in this.outputs) {
                    this.sensors[key] = this.outputs[key];
                    // Updates sensor located in its group for monitoring
                    for(const group in this.sensorGroups) {
                        this.sensorGroups[group].forEach(v => {
                            if(v.key === this.sensors[key].name) {
                                v.value = this.sensors[key];
                            }
                        });
                    }
                }
            },75);
        },
        turnOff() {
            this.openConfirmationWindow("Você gostaria de desligar o respirador?", () => {
                this.$monitorService.getShutdown();
            });
        },
        alternateDisplayMessages() {
            setInterval(() => {
                if(this.messages.length) {
                    let currentMessage = this.messages.shift();
                    this.messages.push(currentMessage);
                }
            },2500);
        },
        openModeMenu (event) {
            this.showMainSideMenu = !this.showMainSideMenu;

            this.showModeMenu = !this.showModeMenu;
            this.$refs['modeMenu'].resetTab();
            if(event){
                this.$refs['modeMenu'].clickedModeMenu();
            }
            this.resetModeMenuParams = !this.resetModeMenuParams;
            this.showPatientInfo = false;
            this.showAlarmMenu = false;
        },
        openAlarm() {
            this.showAlarmMenu = !this.showAlarmMenu;
            this.$refs['alarmConfig'].reset();
            this.showPatientInfo = false;
        },
        openMenu() {
            this.showSubMenu = !this.showSubMenu;
            if(this.showModeMenu) {
                this.showMainSideMenu = false;
            } else {
                this.showMainSideMenu = !this.showMainSideMenu;
            }
            this.showPatientInfo = false;
            this.showAlarmMenu = false;
            this.showModeMenu = false;
        },
        openPatientInfo() {
            this.showPatientInfo = !this.showPatientInfo;
            this.showAlarmMenu = false;
        },
        openConfirmationWindow(confirmationMessage, callback) {
            this.showConfirmationWindow = true;
            this.confirmationMessage = confirmationMessage;
            this.confirmationCallback = callback;
        },
        closeConfirmationWindow() {
            this.showConfirmationWindow = false;
            this.confirmationMessage = '';
            this.confirmationCallback = () => {};
        },
        stopController(){
            this.openConfirmationWindow("Você gostaria de parar o respirador?", () => {
                this.$monitorService.sendStop(() => {
                    this.ventilatorStatus = 'PAUSA';
                });
            })
        },
        startController() {
            this.$monitorService.sendStart(() => {
                this.ventilatorStatus = 'EM OPERAÇÃO';
            });
        },
        openAlarmHistory() {
            this.showAlarmHistory = true;
            this.showAlarmMenu = false;
            this.showPatientInfo = false;
        },
        goToSetup() {
            document.location = './setup.html';
        },
        simulateSockectIo() {

            // Simulating Socket IO messages
            //  This code simulates graphics, sensor and alert messages
            //  It needs to have the mockup server running to works as well


            // Graphics State 1
            let state1 = setInterval(async () => {
                var graphMessage = await this.$monitorService.getGraphics('state-1');
                this.drawHandler(graphMessage);
            }, 2000);

            // Graphics State 2
            let state2 = setInterval(async () => {
                var graphMessage = await this.$monitorService.getGraphics('state-2');
                this.drawHandler(graphMessage);
            }, 3000);

            // Uncomment this to stop alternating graphics
            // setTimeout(() => clearInterval(state1), 3000);
            // setTimeout(() => clearInterval(state2), 3000);

            // Graphics history
            setTimeout(async () => {
                var graphMessage = await this.$monitorService.getGraphics('history');
                graphMessage.graphics.forEach(graph => {
                    this.graphs = this.graphs.map(el => {
                        if (el.title == graph.name) {
                            this.$root.$emit(`history${el.id}`, graph.value);
                        }
                        return el;
                    })
                });
            }, 5000);

            // Sensors State 1
            setInterval(async () => {
                var graphMessage = await this.$monitorService.getSensors('state-1');
                this.reportHandler(graphMessage);
            }, 3000);

            // Sensors State 2
            setInterval(async () => {
                var sensorsMessage = await this.$monitorService.getSensors('state-2');
                this.reportHandler(sensorsMessage);
            }, 4000);



            // Alert messages group 1
            setInterval(() => this.alertHandler({ alerts: [
                { id: 'low_battery', message: "Bateria com pouca energia." }
            ]}), 500);

            // Alert messages group 2
            setInterval(() => this.alertHandler({ alerts: [
                { id: 'high_ppico', message: "P.Pico com valores altos" }, 
                { id: 'high_pressure', message: "Valor de pressão alto." }
            ]}), 750);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'noPower', message: "Sem fonte de energia externa. Consumindo bateria." }
            ]}), 3000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery75', message: "Bateria abaixo de 75%" }
            ]}), 4000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery50', message: "Bateria abaixo de 50%" }
            ]}), 6000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery25', message: "Bateria abaixo de 25%" }
            ]}), 8000);
        }
    }
});
