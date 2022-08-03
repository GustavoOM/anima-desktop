

Vue.component('test-group', {
    props: {
        label: '',
        title: '',
        tests: [],
        imagePath: '',
        mandatory: null,
        firstUpdate: null,
        resetValue: null,
    },
    data: function () {
        return {
            step: 'preparation',
            activeTest: null,
            failure: false,
            success: false,
            errorMessage: '',
            messages: '',
        }
    },
    beforeCreate() {
        this.$monitorService = this.$parent.$monitorService;
    },
    created: function () {
    },
    mounted: function () {
        setTimeout(() => {
            this.pageIncrement();
            this.messages = this.$parent.messages;
        }, 1000);
    },
    updated: function () {
        if(this.firstUpdate) {
            if(this.label === 'DV') {
                this.pageIncrement();
            }
        }
        this.firstUpdate = false;
    },
    template: `
    <div class="test-wrapper">
        <div class="test-preparation test-wrapper">
            <div class="content -expand">
                <div class="title">{{title}}</div>
                <div class="spacer"></div>
                <div v-if="label === 'CM'" class="info-test picture">
                    <!-- <p class="error-msg">Tampe a saída do "Y" das mangueiras para que o circuito fique fechado!</p> -->
                    <p class="error-msg">CONECTE A INALAÇÃO DIRETAMENTE NA EXALAÇÃO</p>
                    <img class="montagem" style="display: none;" v-bind:src="imagePath" alt="montagem respirador">
                </div>
            </div>
            <div v-if="label === 'CM'  && step === 'preparation'" class="btn-group -fEnd">
                <a class="btn -primary" style="color: #000;border: .1rem solid #debd1a;background-color: #debd1a;" v-on:click="pageIncrement" v-visible="true">Avançar</a>
            </div>
        </div>
        <div class="test-list test-wrapper" v-if="step === 'list'">
            <div class="content -expand">
                <div class="spacer"></div>
                <div class="info-test">
                    <ul>
                        <test-item v-for="test in tests" v-bind:key="test.id" v-bind:description="test.description" v-bind:state="test.state" />
                    </ul>
                </div>
            </div>
            <div class="btn-group -fEnd">
                <p class="error-msg" v-if="failure">Sugerimos que não seja a feita a ventilação. Leve à manutenção.</p>
                <a class="btn -primary" style="color: #000;border: .1rem solid #debd1a;background-color: #debd1a;" v-visible="failure" v-on:click="resetTests">Refazer</a>
		        <!-- <a class="btn -outline-alt" v-on:click="runTestAgain" >{{ mandatory ? 'Refazer' : 'Começar'}}</a> -->
                <a class="btn -primary" v-on:click="pageIncrement" v-visible="success">Continuar</a>
            </div>
        </div>
    </div>
    `,
    methods: {
        pageIncrement() {
            if (this.step === "preparation"){
                this.step = 'list';
                if (this.mandatory)
                    this.startNextTest();
            } else {
                this.$parent.$emit('batchEnded');
                setTimeout(() => {}, 1750);
                if (this.mandatory)
                    this.step = 'preparation';
                this.failure = false;
                this.success = false;
                this.errorMessage = '';
            }
        },
        checkActiveTest(testId) {
            this.$monitorService.checkTest(testId, data => {
                if (data.completed) {
                    let test = this.tests.find(t => t.id === testId);
                    if (data.failure){
                        this.$set(test, 'state', 'error');
                        this.failure = true;
                        this.errorMessage = data.message;
                    } else{
                        this.$set(test, 'state', 'check')
                        this.startNextTest();
                    }
                } else {
                    this.checkActiveTest(testId);
                }
            });
        },
        checkAlarmTest(testId) {

            let test = this.tests.find(t => t.id === testId);

            switch(testId) {
                case "eletricA":
                    
                    if(this.$parent.verifyAlert('noPower')) {
                        this.$set(test, 'state', 'error');
                        this.failure = true;
                    } else {
                        this.$set(test, 'state', 'check');
                    }

                break;
                case "batteryAlarm":
                
                    if(this.$parent.verifyAlert('battery25')) {
                        this.$set(test, 'state', 'error');
                        this.failure = true;
                    } else {
                        this.$set(test, 'state', 'check');
                    }
                    
                break;
                case "o2A":
                
                    if(this.$parent.verifyAlert('pO2Min') || this.$parent.verifyAlert('pO2MinWarning')) {
                        this.$set(test, 'state', 'error');
                        this.failure = true;
                    } else {
                        this.$set(test, 'state', 'check');
                    }
                    
                break;
                case "airA":
                
                    if(this.$parent.verifyAlert('pAirMin') || this.$parent.verifyAlert('pAirMinWarning')) {
                        this.$set(test, 'state', 'error');
                        this.failure = true;
                    } else {
                        this.$set(test, 'state', 'check');
                    }
                    
                break;
            }

            this.startNextTest();

        },
        startNextTest() {
            
            let nextTest = this.tests.find(t => t.state === "waiting");
            if (nextTest) {
                this.$set(nextTest, 'state', 'running')
                this.$forceUpdate();
                if(this.label === 'A') {
                    this.activeTest = setInterval(() => {
                        this.checkAlarmTest(nextTest.id)
                    }, 1000);
                } else {
                    console.log("start tests!")
                    this.$monitorService.startTest(nextTest.value, () => {
                        this.checkActiveTest(nextTest.id);
                    })
                }
            } else {
                if(!this.failure)
                    this.success = true;
            }
                       
        },
        resetTests() {
            this.success = false;
            this.failure = false;
            this.tests.forEach(test => {
                this.$set(test, 'state', 'waiting');
            });
            this.$monitorService.resetTest(this.label, () => {
                this.startNextTest();
            });
        },
        runTestAgain() {
            let failedTest = this.tests.find(t => t.state === "error");
            if (failedTest){
                this.$set(failedTest, 'state', 'waiting');
                this.failure = false;
            }
            this.startNextTest();
        }
    }
})
