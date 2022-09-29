/*
============================================================================
== SERVICES DEPENDENCIES ==                                               ==
============================================================================ */
Vue.mixin({
    // Creates instances of desired services.
    // This behaviour is for to implement a kind of 'dependency injection'.
    //   To inject a service in a component, attach the desired service placed 
    //   on $root to some property in the 'beforeCreate' component event.
    beforeMount: function() {

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
============================================================================
== SETUP COMPONENT ==                                                     ==
== Defines all steps to create a configuration for the patient.           ==
== After setup completes, it goes to monitor screen and starts monitoring ==
============================================================================ */
Vue.component('setup', {
    
    // Componente data
    data: function() {
        return {
            modes: [],
            currentStep: 
                'patientTypeStep',
            patientData: {
                age: '',
                gender: '',
                height: 0,
                weight: 0,
                name: '',
                prontuary: ''
            },
            patientParams: {
                mode: 'VCV',
                graphShape: '',
                parameters: []
            },
            patientAlarms: {
                minValues: [],
                maxValues: [],
                othersValues: []
            },
            hourmeter: '',
            messages: [],
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
            }
        }
    },

    // Service injections
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
        this.$socket = this.$root.$socket;
    },

    // Component created event
    created: function() {
        this.populateModes();
        this.$monitorService.getStatus(response => {
            this.hourmeter = response.hourmeter;
            this.$sessionService.set('hourmeter', this.hourmeter);
        });
        this.$socket.registerEvent('alert', this.alertHandler);

        if(appSettings.simulateSocketIo)
            this.simulateSockectIo();

        this.alternateDisplayMessages();
    },

    // HTML template
    template: `
        <div>
            <header-setup 
                v-bind:messages="messages" 
                v-bind:powerIcon="powerIcon" 
                v-bind:batteryIcon="batteryIcon" 
                v-bind:hourmeter="hourmeter">
            </header-setup>
            <patient-type v-show="currentStep == 'patientTypeStep'" />
            <calibration v-show="currentStep == 'calibration'"/>
            <patient-params v-if="currentStep == 'patientParamsStep'" />
            <patient-alarms v-if="currentStep == 'patientAlarmsStep'" />
        </div>
    `,

    // Component methods
    methods: {
        populateModes() {
            this.$monitorService.getModes(modes => {
                this.modes = modes;
            });
        },
        goToMonitoring() {

            this.setPatient(() => {
                this.getSettings(settings => {

                    this.setModeParameters(settings);
                    this.setModeAlarms(settings);

                    this.sendParams(() => {
                        // No need to send alarm here, it is updated in monitoring
                        // this.sendAlarms(() => {

                        this.$sessionService.set('setupConfiguration', {
                            mode: {
                                patient: this.patientData,
                                mode: this.patientParams.mode,
                                graphShape: this.patientParams.graphShape,
                                parameters: this.patientParams.parameters
                            },
                            alarms: this.patientAlarms
                        });

                        document.location = './monitor.html';
                        // });
                    });
                });
            });

        },
        setModeParameters(settings) {

            let modeConfiguration = settings.modes
                .find(mode => mode.mode == this.patientParams.mode).configuration;

            this.patientParams.graphShape = modeConfiguration
                .find(config => config.id == 'shape');

            this.patientParams.parameters = modeConfiguration
                .filter(config => config.id != 'shape');
        },
        setModeAlarms(settings) {

            let modeAlarms = settings.alarms

            this.patientAlarms.minValues = modeAlarms.min
            this.patientAlarms.maxValues = modeAlarms.max

            this.patientAlarms.othersValues = modeAlarms.others;

        },
        setPatient(callback) {
            this.$monitorService.setPatient(
                this.patientData,
                callback
            );
        },
        getSettings(callback) {
            this.$monitorService.getSettings(
                callback
            );
        },
        sendParams(callback) {

            this.$monitorService.sendConfiguration(
                this.patientParams,
                callback
            );

        },
        sendAlarms(callback) {

            this.$monitorService.sendAlarms(
                this.patientAlarms,
                callback
            );
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
                    // and define callback function
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
        alternateDisplayMessages() {
            setInterval(() => {
                if(this.messages.length) {
                    let currentMessage = this.messages.shift();
                    this.messages.push(currentMessage);
                }
            },2500);
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
        simulateSockectIo() {

            // Simulating Socket IO messages
            //  This code simulates graphics, sensor and alert messages
            //  It needs to have the mockup server running to works as well

            // Uncomment this to stop alternating graphics

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'noPower', message: "Sem fonte de energia externa. Consumindo bateria." }
            ]}), 7000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery75', message: "Bateria abaixo de 75%" }
            ]}), 7000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery50', message: "Bateria abaixo de 50%" }
            ]}), 11000);

            setInterval(() => this.alertHandler({ alerts: [
                { id: 'battery25', message: "Bateria abaixo de 25%" }
            ]}), 15000);
        }
    }
});