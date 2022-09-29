

Vue.component('test-audio', {
    props: {
        label: '',
        title: '',
        tests: [],
        imagePath: '',
        mandatory: null
    },
    data: function () {
        return {
            step: 'initial',
            activeTest: null,
            errorMessage: '',
            messages: '',
            isAudioPlaying: false,
            testMessages: {
                initial: 'Aperte o botão acima para tocar o som de alarme.',
                prompt: 'O alarme está tocando. Você escutou o alarme?',
                success: 'Teste bem-sucedido!',
                error: 'Sugerimos que não seja a feita a ventilação. Leve à manutenção.',
            },
        }
    },
    beforeCreate() {
        this.$monitorService = this.$parent.$monitorService;
    },
    created: function () {
    },
    mounted: function () {
        
    },
    template: `
    <div class="test-wrapper">
        <div class="test-preparation test-wrapper">
            <div class="content -expand">
                <div class="title">{{title}}</div>
                <div class="spacer"></div>
                <div v-if="label === 'CM'" class="info-test picture">
                    <!-- <p class="error-msg">Tampe a saída do "Y" das mangueiras para que o circuito fique fechado!</p> -->
                    <p class="error-msg">Conecte a inalação diretamente na exalação.</p>
                    <img class="montagem" style="display: none;" v-bind:src="imagePath" alt="montagem respirador">
                </div>
            </div>
            <div v-if="label === 'CM'  && step === 'preparation'" class="btn-group -fEnd">
                <a class="btn -primary" style="color: #000;border: .1rem solid #debd1a;background-color: #debd1a;" v-on:click="pageIncrement" v-visible="true">Avançar</a>
            </div>
        </div>
        <div class="test-list test-wrapper">
            <div class="content -expand">
                <div class="spacer"></div>
                <div class="info-test">
                    <div class="btn-group" style="justify-content:center;">
                        <a class="btn -primary" v-on:click="playSound" style="margin:0;" v-bind:class="[ isAudioPlaying ? '-disable' : '']" >Tocar Alarme</a>
                    </div>
                    <p class="audio-msg" style="background-color:#d1d2dc;color:#0d1242;"v-if="step !== 'error' && step !== 'success'">{{testMessages[step]}}</p>
                    <p class="success-msg" v-if="step === 'success'">{{testMessages[step]}}</p>
                    <p class="error-msg" v-if="step === 'error'">{{testMessages[step]}}</p>
                    <div class="btn-group" style="justify-content:center;margin-top:2.5em;">
                        <a class="btn -outline" style="margin-right:1em;" v-on:click="confirm(true)" v-visible="step === 'prompt'">Sim</a>
                        <a class="btn -outline" style="margin:0;" v-on:click="confirm(false)" v-visible="step === 'prompt'">Não</a>
                    </div>
                </div>
            </div>
            <div class="btn-group -fEnd">
                <a class="btn -primary" v-on:click="pageIncrement" v-visible="step === 'success'">Continuar</a>
            </div>
        </div>
    </div>
    `,
    methods: {
        pageIncrement() {

            this.$parent.$emit('batchEnded');

        },
        playSound() {

            if(this.isAudioPlaying) {
                return;
            }

            // this.step = 'prompt';
            //     this.isAudioPlaying = true;
            //     setTimeout(() => {
            //         this.isAudioPlaying = false;
            //     }, 3000);
            //     return;

            this.step = 'prompt';
            this.isAudioPlaying = true;
            setTimeout(() => {
                this.isAudioPlaying = false;
            }, 3000);

            this.$monitorService.sendTestSound(data => { });

        },
        confirm(response) {
            if(response) {
                this.step = 'success';
            } else {
                this.step = 'error';
            }
        }
    }
})
