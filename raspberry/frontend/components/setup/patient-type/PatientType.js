Vue.component('patient-type', {
    props: {},
    data: function() {
        return {
            patientAlarms: this.$parent.patientAlarms,
            patientParams: this.$parent.patientParams,
            patientData: this.$parent.patientData,
            patientType: this.$parent.patientType,

            calibration: {
                text: [
                    'CALIBRAÇÃO EM PROGRESSO',
                    'CALIBRAÇÃO BEM-SUCEDIDA!'
                ],
                textIndex: 0,
                first: false,
                isActive: false,
                progress: 0,
                done: false,
                isMandatory: true,
                interval: setInterval(() => {})
            },
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
        
    },
    template: `

    <div class="new-patient">
        <div class="patient-data">
            <div class="content -start">
                <div class="data">
                    <div class="title">Novo Paciente</div>
                    <div class="item">
                        <p class="description">Idade:</p>
                        <div class="value">
                            <input type="radio" name="age" id="age2" value="adult" v-model="patientData.age" hidden>
                            <label for="age2" style="padding-left:3.6rem; padding-right:3.6rem;">Adulto</label>
                            <input type="radio" name="age" id="age1" value="child" v-model="patientData.age" hidden>
                            <label for="age1" style="padding-left:3.6rem; padding-right:3.6rem;">Pediátrico</label>
                        </div>
                    </div>
                    <div class="item">
                        <p class="description">Sexo:</p>
                        <div class="value">
                            <input type="radio" name="genre" id="genre1" value="male" v-model="patientData.gender" hidden>
                            <label for="genre1" style="padding-left:3.6rem; padding-right:3.6rem;">Masculino</label>
                            <input type="radio" name="genre" id="genre2" value="female" v-model="patientData.gender" hidden>
                            <label for="genre2" style="padding-left:3.6rem; padding-right:3.6rem;">Feminino</label>
                        </div>
                    </div>
                    <div class="item" style="min-width: 27rem;">
                        <p class="description">Altura:</p>
                        <div class="value">
                            <input class="v-board" data-kioskboard-type="numpad" type="text" value="0" name="altura" id="altura" value="">
                            <span style="text-transform: lowercase;">cm</span>
                        </div>
                    </div>
                    <div class="item" style="margin-left: 2.4rem; min-width: 27rem;">
                        <p class="description">Peso:</p>
                        <div class="value">
                            <input class="v-board" data-kioskboard-type="numpad" type="text" value="0" name="peso" id="peso" value="">
                            <span style="text-transform: lowercase;">Kg</span>
                        </div>
                    </div>
                </div>
                <div class="data">
                    <div class="title">Informações do Paciente</div>
                    <div class="item" style="width: 100%;">
                        <p class="description">Nome:</p>
                        <div class="value">
                            <input class="v-board" data-kioskboard-type="all" data-kioskboard-specialcharacters="false"
                                    style="width: 100%; height: 7.2rem;" type="text" name="nome" id="nome" spellcheck="false">
                        </div>
                    </div>
                    <div class="item" style="width: 100%;">
                        <p class="description">Prontuário:</p>
                        <div class="value">
                            <input class="v-board" data-kioskboard-type="all" data-kioskboard-specialcharacters="false"
                                     style="width: 100%;" type="text" name="prontuario" id="prontuario" spellcheck="false">
                        </div>
                    </div>
                    
                </div>
            </div>
        </div>
        <div class="btn-group">
            <button id="continue1" class="btn -continue" v-bind:class="[!patientData.age ? '-disable' : '']" v-on:click="goToCalibration">Continuar</button>
        </div>
    </div>
        
    `,
    methods: {

        clearSelection() {
            this.selected = '';
        },

        getImageFromValue(value) {
            colorSuffix = this.patientData.age == value ? '-dark' : '';
            return `./images/icon-${value}${colorSuffix}.svg`;
        },

        // goToPatientData() {
        //     this.$parent.patientData = this.patientData;
        //     this.$parent.currentStep = 'patientDataStep';
        // }

        goToCalibration() {

            if (!this.patientData.age)
                return;

            this.patientData.height = document.getElementById('altura').value;
            this.patientData.weight = document.getElementById('peso').value;
            this.patientData.name = document.getElementById('nome').value;
            this.patientData.prontuary = document.getElementById('prontuario').value;
            this.$parent.currentStep = "calibration";

        },
    }
})
