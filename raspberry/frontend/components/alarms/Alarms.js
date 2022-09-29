Vue.component('alarms-config', {
    props: {
        alarms: {},
    },
    data: function() {
        return {
            someSelected: false,
            showContinueTask: {},
            selectedParam: {},
            nightMode: false,
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function() {
        this.listenRequiredEvents();
        this.originalAlarms = this.alarms;
        
        this.$monitorService.setNightMode(this.nightMode);
    },
    template: `
        <div class="alarms-config">
            <span class="main-title">Configuração de Alarmes</span>

            <div class="min-max-section">
                <div class="min-section">
                    <span>MIN</span>
                    <param-input-side
                        v-for="parameter in alarms.minValues" 
                        v-bind:parameter="parameter"
                        v-bind:selectedParam="selectedParam"
                        v-bind:ref="parameter.id"
                        v-bind:isAlarm="true"> 
                    </param-input-side>
                </div>
                <div class="max-section">
                    <span>MAX</span>
                    <param-input-side
                        v-for="parameter in alarms.maxValues" 
                        v-bind:parameter="parameter"
                        v-bind:selectedParam="selectedParam"
                        v-bind:ref="parameter.id"
                        v-bind:isAlarm="true"> 
                    </param-input-side>
                </div>
            </div>

            <div class="others-section">
                <param-input-side
                    v-for="parameter in alarms.othersValues" 
                    v-bind:parameter="parameter"
                    v-bind:limit-label="parameter.limitLabel" 
                    v-bind:selectedParam="selectedParam"
                    v-bind:ref="parameter.id"
                    v-bind:isAlarm="true"> 
                </param-input-side>
            </div>

            <div class="bottom">
                <button class="btn -outline -nightmode" v-bind:class="[nightMode ? '-active' : '']" v-on:click="toggleNightMode()">
                    <!-- <img v-show="nightMode" v-bind:src="getImageFromIconLabel('power-sleep')"> --> 
                    {{nightMode ? 'Modo Noturno' : 'Modo Diurno'}}
                </button>
                <button class="btn -outline"v-on:click="cancelAlarmsConfig()">Cancelar</button>
                <button class="btn -accept" v-on:click="confirmAlarmsConfig()" v-visible="!someSelected" >Aceitar</button>
            </div>

        </div>
    `,
    methods: {
        toggleNightMode() {
            this.nightMode = !this.nightMode;
            this.$monitorService.setNightMode(this.nightMode);
        },
        reset() {
            this.$emit('resetAlarmBounds');
            this.selectedParam = {};
        },
        listenRequiredEvents() {

            this.$on('selection', parameter => {
                this.selectedParam = parameter;
            });

            // Notifies parameters edition to show/hide accept
            this.$on('parameterSelected', paramSelected => {
                if(!paramSelected) {
                    this.showContinueTask = setTimeout(() => this.someSelected = false, 150);
                }
                else {
                    clearTimeout(this.showContinueTask);
                    this.someSelected = true;
                }
            });
        },
        confirmAlarmsConfig() {
            this.$emit('updateDefaultAlarmBounds');
            
            this.sendAlarmsConfig(this.alarms);
            this.selectedParam = {};
        },
        cancelAlarmsConfig() {
            this.$parent.showAlarmMenu = false;
            this.$emit('resetAlarmBounds');
        },
        sendAlarmsConfig(alarmsConfig) {

            // console.table(alarmsConfig.minValues.map(a => Object.assign({},a)));
            // console.table(alarmsConfig.maxValues.map(a => Object.assign({},a)));
            // console.table(alarmsConfig.othersValues.map(a => Object.assign({},a)));

            this.$monitorService.sendAlarms(alarmsConfig, () => {
                this.$parent.showAlarmMenu = false;
                // this.showAlarmsConfigChangeInfoMessage();
                this.$parent.updateSessionData();
            });
            
        },
        // Receives an array of alarms with id and value to update
        updateBoundaries(alarms) {
            alarms.forEach(alarm => {
                if(alarm.id === 'pInsMax') {
                    this.$refs[`${alarm.id}`][0].setCurrentDefault( (Number(alarm.value) + 5).toString() );
                    this.$refs[`${alarm.id}`][0].parameter.range[0] = Number(alarm.value);
                }
                if(alarm.id === 'pInsMin') {
                    if(Number(this.$refs[`${alarm.id}`][0].parameter.value) > Number(alarm.value)) {
                        this.$refs[`${alarm.id}`][0].setCurrentDefault( (Number(alarm.value)).toString() );
                    }
                    this.$refs[`${alarm.id}`][0].parameter.range[1] = Number(alarm.value);
                }
            });
            setTimeout(() => {
                this.sendAlarmsConfig(this.alarms);
            }, 75);
        },
        showAlarmsConfigChangeInfoMessage() {
            setTimeout(() => {
                alert("As alterações nos alarmes foram aplicadas com sucesso.");
            },500);
        }
    }

});
