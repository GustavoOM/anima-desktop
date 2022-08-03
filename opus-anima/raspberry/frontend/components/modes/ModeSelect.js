Vue.component('mode-select', {
    props: {
        age: '',
        modes: []
    },
    data: function () {
        return {
            modeSelected: {},
            showList: false,
            someSelected: false,
            showContinueTask: {}
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function () {
        this.listenRequiredEvents();
    },
    template: `
        <div class="mode" v-bind:class="{ '-active': showList }" v- v-on:mouseleave="cancelParameters()" ref="modeSelect">
            <div class="inner" v-on:click="toggleModeSelect">
                <p>Modo Atual:</p>
                <span class="selected">{{ modeSelected.mode }}</span>
                <span class="age">{{ mapAgeGroupToLabel(age) }}</span>
            </div>
            <button class="btn -dropdown" v-on:click="toggleModeSelect">
                <img src="./images/icon-menu-down-dark.svg" alt="dropdown">
            </button>
            <ul class="list" v-if="showList">
                <li v-for="mode in modes" >
                    <span>{{mode.mode}}</span>
                    <div class="modeParams">
                        <div class="top">{{mode.mode}}</div>
                        <div class="content">
                            <shape-options 
                                v-if="mode.graphShape"
                                v-model="mode.graphShape">
                            </shape-options>
                            <param-input 
                                v-for="parameter in mode.parameters"
                                v-bind:parameter="parameter" >
                            </param-input>
                        </div>
                        <div class="bottom">
                            <button class="btn -outline"v-on:click="cancelParameters()">Cancelar</button>
                            <button class="btn -accept" v-on:click="confirmParameters(mode)" v-visible="!someSelected" >Aceitar</button>
                        </div>
                    </div>
                </li>
            </ul>
        </div>
    `,
    methods: {
        listenRequiredEvents() {

            // Notifies mode changes
            this.$root.$on("modeLoaded", mode => {
                this.modeSelected = mode;
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
        toggleModeSelect() {
            this.showList = !this.showList;
        },
        confirmParameters(mode) {
            this.sendParameters(mode);
        },
        cancelParameters() {
            this.showList = false;
        },
        sendParameters(mode) {
            this.$monitorService.sendConfiguration(mode, () => {
                this.showList = false;
                this.notifyModeChange(mode);
                this.showModeChangeInfoMessage();
            });
        },
        notifyModeChange(mode) {
            let modeToChange = JSON.parse(JSON.stringify(mode));
            this.$parent.$emit('changeMode', modeToChange);
        },
        showModeChangeInfoMessage() {
            setTimeout(() => {
                this.restoreDefaultValues();
            },500);
        },
        anySelected(mode) {
            return mode.parameters.some(p => p.selected);
        },
        restoreDefaultValues() {
            this.modes.forEach(mode => { 
                if(mode.graphShape)
                    mode.graphShape.value = mode.graphShape.default;
                mode.parameters.forEach(param => param.value = param.default);
            });
        }
    }
})