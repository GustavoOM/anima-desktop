Vue.component('patient-alarms', {
    props: {
        
    },
    data: function() {
        return {
            patientAlarms: this.$parent.patientAlarms,
            patientParams: this.$parent.patientParams,
            patientData: this.$parent.patientData,

            someSelected: false,
            showContinueTask: {},

            paramSelected: {}
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
    },
    created() {
        this.listenRequiredEvents();
    },
    template: `

        <div class="patient-alarms">
            <div class="content -start">
                <div class="title">Novo Paciente</div>
                
                <patient-data-view v-bind:patient-data="patientData" v-bind:mode="patientParams.mode"/>

                <div class="title">Alarmes</div>

                <div id="paramWrapper" class="params min-max-pairs" >
                    <patient-param-input limit-label="min"
                        v-for="param in patientAlarms.minValues" 
                        v-bind:parameter="param"
                        v-bind:paramSelected="paramSelected"> 
                    </patient-param-input>
                </div>
                <div id="paramWrapper" class="params min-max-pairs" >
                    <patient-param-input limit-label="max"
                        v-for="param in patientAlarms.maxValues" 
                        v-bind:parameter="param"
                        v-bind:paramSelected="paramSelected"> 
                    </patient-param-input>
                </div>
                <div id="paramWrapper" class="params" >
                    <patient-param-input
                        v-for="param in patientAlarms.othersValues" 
                        v-bind:parameter="param"
                        v-bind:limit-label="param.limitLabel"
                        v-bind:paramSelected="paramSelected"> 
                    </patient-param-input>
                </div>

            </div>
            <div class="btn-group">
                <button id="back1" class="btn -cancel" v-on:click="goBackToParams()">Voltar</button>
                <button id="continue2" class="btn -continue" v-on:click="goToMonitoring" v-visible="!someSelected" >Continuar</button>
            </div>
        </div>
        
    `,
    methods: {
        listenRequiredEvents() {
            this.$on('parameterSelected', param => {
                this.paramSelected = param;
            });
        },
        goBackToParams() {
            this.$parent.currentStep = "patientParamsStep";
        },
        goToMonitoring() {

            this.sendParams(() => {
                this.sendAlarms(() => {

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
                })
            });

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
        }
    }
})