Vue.component('calibration', {
    props: {
        
    },
    data: function() {
        return {
            text: [
                'CALIBRAÇÃO EM PROGRESSO',
                'CALIBRAÇÃO BEM-SUCEDIDA!',
                'FALHA NA CALIBRAÇÃO',
            ],
            done: false,
            failure: false,
            first: true,
            textIndex: 0,
            isActive: false,
            progress: 0,
            isMandatory: true,
            interval: setInterval(() => {}),
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function() {
    },
    template: `

    <div class="new-patient">
        <div class="patient-data">
            <div class="content -start">
                <div class="data" style="width: 100%;display:block">
                    <div class="title">Novo Paciente</div>
                    <div class="item">
                        <p class="description">Calibração do fluxo de exalação (Obrigatório):</p>
                        <div class="btn-group" style="justify-content: center; margin-top: 4.75rem;" v-if="first">
                            <p class="error-msg">Conecte a inalação diretamente na exalação</p>
                        </div>
                    </div>
                    <div class="item" style="width: 100%;" v-bind:class="[first ? '-hidden' : '']">
                        <div style="display: flex; flex-direction: row; align-items: flex-start; justify-content: flex-start;">
                            <span class="icon-loader" v-if="isActive === true"></span>
                            <img class="icon-check" style="display:hidden;" v-if="done === true" src="./images/check-bold.svg" alt="success">
                            <img class="icon-error" style="display:hidden;" v-if="failure" src="./images/alert-circle-red.svg" alt="error">
                            <span class="description" style="margin-left: 16px;">{{text[textIndex]}}</span>
                        </div>
                        <div class="progress-bar">
                            <div class="bar" :style="{width: progress*10 + '%'}"></div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <div class="btn-group">
            <!-- <button id="back1" class="btn -cancel" v-on:click="goBackPatient()">Voltar</button> -->
            <button class="btn -calib" v-if="first" v-on:click="startCalibration">Iniciar</button>
            <button class="btn -calib" v-if="failure" v-on:click="startCalibration">Reiniciar</button>
            <button id="continue1" class="btn -continue" v-if="done" v-on:click="goToMonitoring">Continuar</button>
        </div>
    </div>
        
        
    `,
    methods: {
        goBackPatient() {
            this.$parent.currentStep = "patientTypeStep";
        },
        goToMonitoring() {
            if (this.isActive || !this.done)
                return;

            this.$parent.goToMonitoring();
        },
        startCalibration() {

            if (this.isActive)
                return;

            // this.first = false;
            // this.isActive = true;
            // this.done = false;
            // this.failure = false;
            // this.textIndex = 0;
            // this.progress = 0;
            // this.interval = setInterval(() => {
            //     this.checkCalibration();
            // }, 500);
            // return;

            this.sendStartCalibration(() => {
                this.first = false;
                this.isActive = true;
                this.done = false;
                this.failure = false;
                this.textIndex = 0;
                this.progress = 0;
                this.interval = setInterval(() => {
                    this.checkCalibration();
                }, 500);
            });

        },
        checkCalibration() {

            // this.progress += 1;   
            // if(this.progress === 10) {
            //     this.isActive = false;
            //     this.done = true;
            //     this.failure = false;
            //     this.textIndex = 1;
            //     clearInterval(this.interval);
            // } 
            // return;

            this.$monitorService.checkCalibration(data => {
                if(data.progress === -1) {
                    this.isActive = false;
                    this.done = false;
                    this.failure = true;
                    this.textIndex = 2;
                    clearInterval(this.interval);
                } else {
                    this.progress = data.progress;
                    
                    if(true) {
                        clearInterval(this.interval);
                        this.isActive = false;
                        this.done = true;
                        this.textIndex = 1;
                    } 
                }
            });

        },
        sendStartCalibration(callback) {

            this.$monitorService.sendStartCalibration(
                callback
            );
        }
    }
})


