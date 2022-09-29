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


Vue.component('test-cycle', {
    props: {
    },
    data: function () {
        return {
            firstReadSerial: false,

            tests: {cm: [], co: [], dm: [], do:[], dv:[]},
            label: [ "DM", "DV", "CM", "S", "CO", "DO"],
            state: 0,
            mandatory: true,
            
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
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
        this.$socket = this.$root.$socket;

        this.$monitorService.getTestData(response => {
            let allTests = response.tests;
            allTests.forEach(test => {
                test.state = "waiting";
            });

            this.tests.cm = allTests.filter(test => test.cycle === "CM");
            this.tests.co = allTests.filter(test => test.cycle === "CO");
            this.tests.dm = allTests.filter(test => test.cycle === "DM");
            this.tests.dv = allTests.filter(test => test.cycle === "DV");
            this.tests.do = allTests.filter(test => test.cycle === "DO");
            
            console.log("ALL TESTS = ");
            console.log(this.tests);
        })
    },
    created: function () {
        this.$on("batchEnded", groupId => {
            this.move();
        })

        this.$monitorService.getStatus(response => {
                        this.hourmeter = response.hourmeter;
                        this.$sessionService.set('hourmeter', this.hourmeter);
                    });
                    this.$socket.registerEvent('alert', this.alertHandler);
            
                    if(appSettings.simulateSocketIo)
                        this.simulateSockectIo();
            
                    this.alternateDisplayMessages();
                    this.$monitorService.sendInitialParameters(() => { });
    },
    template: `
        <div>
            <header-setup 
                v-bind:messages="messages" 
                v-bind:powerIcon="powerIcon" 
                v-bind:batteryIcon="batteryIcon" 
                v-bind:hourmeter="hourmeter">
            </header-setup>
            <div class="test-wrapper">
                <test-group title="Autotestes Rápidos" v-bind:label="'DM'" v-bind:mandatory="true" v-if="label[state] === 'DM'"
                    v-bind:tests="tests.dm" image-path="./images/montagem-respirador.jpg"> </test-group>
                <test-group title="Autotestes" v-bind:label="'DV'" v-bind:firstUpdate="true" v-bind:mandatory="true" v-if="label[state] === 'DV'"
                    v-bind:tests="tests.dv" image-path="./images/montagem-respirador.jpg"> </test-group>    
                <test-group title="Autotestes Conectados" v-bind:label="'CM'"  v-bind:mandatory="true" v-if="label[state] === 'CM'"
                    v-bind:tests="tests.cm" image-path="./images/montagem-respirador.jpg"> </test-group>
                <test-audio title="Teste de Som de Alarme" v-bind:label="'S'" v-bind:ref="'autoTests'" v-bind:mandatory="true"  v-if="label[state] === 'S'"> </test-audio>
                <!--
		        <test-group title="Ciclo de Testes Conectado (Opcionais)" v-bind:ref="'CO'" v-bind:label="'CO'"  v-bind:mandatory="true" v-if="label[state] === 'CO'"
                    v-bind:tests="tests.co" image-path="./images/montagem-respirador.jpg"> </test-group> 
                <test-group title="Auto Testes" v-bind:label="'A'"  v-bind:mandatory="true" v-bind:ref="'autoTests'"  v-if="label[state] === 'A'"
                    v-bind:tests="tests.a" image-path="./images/montagem-respirador.jpg"> </test-group>
                <test-group title="Ciclo de Testes Desconectado" v-bind:label="'DO'" v-bind:mandatory="false" v-else-if="label[state] === 'DO'"
                    v-bind:tests="tests.do" image-path="./images/montagem-respirador.jpg"> </test-group> -->
            </div>
        </div>
    `,
    methods: {
        move(){
            this.state += 1;

            if (this.state >= 4)
                document.location = './setup.html';
        },
        verifyAlert(id) {
            if(this.messages.find(alert => alert.id === id)) {
                return true;
            } 
            return false;
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
            if(!this.firstReadSerial){
                setTimeout(() => {
                    console.log(this.$refs)
                    this.$refs['autoTests'].pageIncrement();
                    this.$refs['autoTests'].messages = this.messages;
                }, 500);
                this.firstReadSerial = true;
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
})
