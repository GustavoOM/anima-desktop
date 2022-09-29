Vue.component('patient-data', {
    props: {
        
    },
    data: function() {
        return {
            modes: this.$parent.modes,
            patientAlarms: this.$parent.patientAlarms,
            patientParams: this.$parent.patientParams,
            patientData: this.$parent.patientData,
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function() {
    },
    template: `

    <div class="patient-data">
        <div class="content -start">
            <div class="title">Novo Paciente</div>
            <div class="data">
                <patient-data-item 
                    description="Paciente" 
                    v-bind:icon-label="patientData.age"
                    v-bind:value-label="mapAgeGroupToLabel(patientData.age)" 
                />
                <div class="item">
                    <p class="description">Sexo:</p>
                    <div class="value">
                        <input type="radio" name="genre" id="genre1" value="male" v-model="patientData.gender" hidden>
                        <label for="genre1" style="padding-left:3.6rem; padding-right:3.6rem;">Masculino</label>
                        <input type="radio" name="genre" id="genre2" value="female" v-model="patientData.gender" hidden>
                        <label for="genre2" style="padding-left:3.6rem; padding-right:3.6rem;">Feminino</label>
                    </div>
                </div>
                <div class="item" >
                    <p class="description">Altura:</p>
                    <div class="value">
                        <input type="number" value="0" name="altura" id="altura" v-model="patientData.height">
                        <span>cm</span>
                    </div>
                </div>
                <div class="item" style="margin-left: 2.4rem;">
                    <p class="description">Peso:</p>
                    <div class="value">
                        <input type="number" value="0" name="peso" id="peso" v-model="patientData.weight">
                        <span>Kg</span>
                    </div>
                </div>
            </div>
            <div class="title">Modo Ventilatório</div>
            <div class="data">
                <div class="item">
                    <div class="value -wrapped ">

                        <div v-for="$mode in modes" style="display: flex;">
                            <input type="radio" name="mode" v-bind:id="'mode'+$mode.id" v-bind:value="$mode.label" v-model="patientParams.mode" hidden>
                            <label class="mode-option -spaced" v-bind:for="'mode'+$mode.id" >
                                {{ $mode.label }}
                            </label>
                        </div>

                    </div>
                </div>
            </div>
        </div>
        <div class="btn-group" >
            <button id="back1" class="btn -cancel" v-on:click="goBackNewPatient">Voltar</button>
            <button id="continue2" class="btn -continue" v-if="patientParams.mode" v-on:click="goToPatientParams">Continuar</button>
            <button id="continue2" class="btn -continue" v-if="!patientParams.mode" >Selecione o Modo Ventilatório</button>
        </div>
    </div>
        
    `,
    methods: {
        goBackNewPatient() {
            this.$parent.currentStep = "patientTypeStep";
        },
        goToPatientParams() {

            this.setPatient(() => {
                this.getSettings(settings => { 

                    this.setModeParameters(settings);
                    this.setModeAlarms(settings);
                            
                    this.navigateToPatientParams();
                });
            });
    
        },
        navigateToPatientParams() {
            this.$parent.patientData = this.patientData;
            this.$parent.patientParams = this.patientParams;
            this.$parent.patientAlarms = this.patientAlarms;
            this.$parent.currentStep = "patientParamsStep";
        },
        setModeParameters(settings) {

            // The modes attribute seems to add spaces for no particular reason
            if(this.patientParams.mode == 'PSV') {
                this.patientParams.mode = 'PSV';
            }

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
        }
    }
})


