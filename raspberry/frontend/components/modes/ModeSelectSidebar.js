Vue.component('mode-select-sidebar', {
    props: {
        modes: [],
        showModeMenu: false,
        triggerParamsIds: [],
        backupParamsIds: [],
    },
    data: function () {
        return {
            modesTemp: [ 
                {
                    modeRequest: 'PSV',
                    mode: 'SIMV(VC) + PS',
                    ref: 'SIMVVC',
                    parameters: []
                },
                {
                    modeRequest: 'PSV',
                    mode: 'SIMV(PC) + PS',
                    ref: 'SIMVPC',
                    parameters: []
                },
            ],
            modeSelected: {},
            currentTab: 'DEFINIÇÕES',
            selectedParam: {},
            someSelected: false,
            showContinueTask: {},
            itemModeSelected: '',
            updateOnce: true
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function () {
        this.listenRequiredEvents();
    },
    updated() {
        if(this.updateOnce) {
            let psvMode = this.modes.find(m => m.mode == 'PSV' );
            this.modesTemp[0].parameters = [...psvMode.parameters];
            this.modesTemp[0].parameters.push({ id: 'vTidal', name: 'V Tidal', default: 500, range: [50, 2000], step: 10, unit: 'ml', value: '500', precision: 0 });
            this.modesTemp[0].parameters.push({ id: 'Freq', name: 'Freq', default: 20, range: [1,80], step: 1, unit: 'BPM', value: '20', precision: 0 });
        
            this.modesTemp[1].parameters = [...psvMode.parameters];
            this.modesTemp[1].parameters.push({ id: 'pCon', name: 'Pressão', default: 20, range: [5,70], step: 1, unit: 'cmH2O', value: '20', precision: 0 });
            this.modesTemp[1].parameters.push({ id: 'Freq', name: 'Freq', default: 20, range: [1,80], step: 1, unit: 'BPM', value: '20', precision: 0 });
            this.updateOnce = false;
        }
    },
    template: `
        <div class="mode-menu" v- ref="modeSelect" v-show="showModeMenu">
            <ul class="list">
                <li v-for="mode in modes" v-bind:class="[mode.mode === itemModeSelected ? '-selected' : '']">
                    <p class="mode-title" v-on:click="changeModeItem(mode.mode)">{{mode.mode}}</p>
                    <div class="params" v-bind:class="[mode.mode === itemModeSelected ? '-show' : '-hide']">
                        <div class="tab" v-show="currentTab === 'DEFINIÇÕES'">
                            <div class="top">
                                <button class="inactiveTab" v-bind:class="[mode.mode === 'PSV' ? '' : 'hide-backup']" v-on:click="toogleTab('BACKUP')"><span>Backup</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('TRIGGER')"><span>Trigger</span></button>
                                <button class="activeTab"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="!triggerParamsIds.includes(parameter.id) && !backupParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam"
                                     >
                                </param-input-side>
                                <shape-options 
                                    v-if="mode.graphShape"
                                    v-model="mode.graphShape">
                                </shape-options>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParameters(mode)" v-visible="!someSelected" >Aceitar</button>
                            </div>
                        </div>
                        
                        <div class="tab" v-show="currentTab === 'TRIGGER'" v-bind:class="[mode.mode === itemModeSelected ? '-active' : '']">
                            <div class="top">
                                <button class="inactiveTab" v-bind:class="[mode.mode === 'PSV' ? '' : 'hide-backup']" v-on:click="toogleTab('BACKUP')"><span>Backup</span></button>
                                <button class="activeTab"><span>Trigger</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('DEFINIÇÕES')"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="triggerParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam" 
                                     >
                                </param-input-side>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParameters(mode)" v-visible="!someSelected" >Aceitar</button>
                            </div>
                        </div>

                        <div class="tab" v-show="currentTab === 'BACKUP'" v-bind:class="[mode.mode === itemModeSelected ? '-active' : '']">
                            <div class="top">
                                <button class="activeTab"><span>Backup</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('TRIGGER')"><span>Trigger</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('DEFINIÇÕES')"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="backupParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam" 
                                     >
                                </param-input-side>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParameters(mode)">Aceitar</button>
                            </div>
                        </div>
                    </div>
                </li>

                <li v-for="mode in modesTemp"  v-bind:class="[mode.mode === itemModeSelected ? '-selected' : '']">
                    <p class="mode-title" v-on:click="changeModeItem(mode.mode)">{{mode.mode}}</p>
                    <div class="params" v-bind:class="[mode.mode === itemModeSelected ? '-show' : '-hide']">
                        <div class="tab" v-show="currentTab === 'DEFINIÇÕES'">
                            <div class="top">
                                <button class="inactiveTab" v-on:click="toogleTab('BACKUP')"><span>Backup</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('TRIGGER')"><span>Trigger</span></button>
                                <button class="activeTab"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="!triggerParamsIds.includes(parameter.id) && !backupParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam"
                                     >
                                </param-input-side>
                                <shape-options 
                                    v-if="mode.graphShape"
                                    v-model="mode.graphShape">
                                </shape-options>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParametersTemp(mode)" v-visible="!someSelected" >Aceitar</button>
                            </div>
                        </div>
                        
                        <div class="tab" v-show="currentTab === 'TRIGGER'">
                            <div class="top">
                                <button class="inactiveTab" v-on:click="toogleTab('BACKUP')"><span>Backup</span></button>
                                <button class="activeTab"><span>Trigger</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('DEFINIÇÕES')"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="triggerParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam" 
                                     >
                                </param-input-side>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParametersTemp(mode)" v-visible="!someSelected" >Aceitar</button>
                            </div>
                        </div>

                        <div class="tab" v-show="currentTab === 'BACKUP'">
                            <div class="top">
                                <button class="activeTab"><span>Backup</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('TRIGGER')"><span>Trigger</span></button>
                                <button class="inactiveTab" v-on:click="toogleTab('DEFINIÇÕES')"><span>Definições</span></button>
                            </div>
                            <div class="content">
                                <param-input-side
                                    v-for="(parameter, i) in mode.parameters"
                                    v-show="backupParamsIds.includes(parameter.id)"
                                    v-bind:parameter="parameter"
                                    v-bind:mode="mode.mode"
                                    v-bind:selectedParam="selectedParam" 
                                     >
                                </param-input-side>
                            </div>
                            <div class="bottom">
                                <button class="btn -outline" v-on:click="openModeMenu()">Cancelar</button>
                                <button class="btn -accept" v-on:click="confirmParametersTemp(mode)">Aceitar</button>
                            </div>
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
        resetTab() {
            this.toogleTab('DEFINIÇÕES');
        },
        openModeMenu() {
            this.$parent.openModeMenu();
        },
        changeModeItem(mode) {
            if(mode != this.itemModeSelected) {
                this.resetTab();
            }
            this.restoreValuesFromMode(mode);
            this.itemModeSelected = mode;
            this.selectedParam = {};
        },
        clickedModeMenu() {
            this.itemModeSelected = '';
            this.selectedParam = {};
            this.restoreAllPreviousValues();
        },
        toggleModeSelect() {
            this.showList = !this.showList;
        },
        toogleTab(tab) {
            this.currentTab = tab;
        },
        confirmParameters(mode) {
            this.sendParameters(mode);
            this.openModeMenu();
        },
        confirmParametersTemp(mode) {
            let modeTemp = mode.mode;
            let paramsTemp = mode.parameters;
            let paramsFake = ['Freq', 'vTidal', 'pCon'];
            mode.mode = mode.modeRequest;
            mode.parameters = mode.parameters.filter((p) => { return !paramsFake.includes(p.id) });
            
            this.$monitorService.sendConfiguration(mode, () => {
                this.showList = false;
                mode.mode = modeTemp;
                mode.parameters = paramsTemp;
                this.notifyModeChange(mode);
                this.updateValuesFromMode(mode.mode);
                this.showModeChangeInfoMessage();
            });

            this.openModeMenu();
        },
        sendParameters(mode) {
            this.$monitorService.sendConfiguration(mode, () => {
                this.showList = false;
                this.notifyModeChange(mode);
                this.updateValuesFromMode(mode.mode);
                this.showModeChangeInfoMessage();
            });
        },
        notifyModeChange(mode) {
            let modeToChange = JSON.parse(JSON.stringify(mode));
            this.$parent.$emit('changeMode', modeToChange);
        },
        showModeChangeInfoMessage() {
            setTimeout(() => {
                this.restoreToInitialDefault();
            },500);
        },
        anySelected(mode) {
            return mode.parameters.some(p => p.selected);
        },
        updateValue(selectedMode, parameter) {
            this.$emit('updateValuesFromMode', selectedMode, parameter);
        },
        // Set new default value for parameter from confirmed mode parameters
        updateValuesFromMode(selectedMode) {
            this.$emit('updateValuesFromMode', selectedMode);
            // let mode = this.modes.find(mode => { return mode.mode === selectedMode })
            // if(mode) {
            //     mode.parameters.forEach(param => {
            //         this.$refs[`${selectedMode}-${param.name}`][0].setCurrentDefault();
            //     });
            // } else {
            //     let mode = this.modesTemp.find(mode => { return mode.mode === selectedMode });
            //     mode.parameters.forEach(param => {
            //         this.$refs[`${mode.ref}-${param.name}`][0].setCurrentDefault();
            //     });
            // }
        },
        // Restore param to default value when another mode is selected from menu
        restoreValuesFromMode(selectedMode) {
            this.$emit('restoreValueFromMode', selectedMode);
            // let mode = this.modes.find(mode => { return mode.mode === selectedMode })
            // if(mode) {
            //     mode.parameters.forEach(param => {
            //         console.log("Mode: " + mode.mode)
            //         this.$refs[`${mode.mode}-${param.name}`][0].restoreParam();
            //     });
            // } else {
            //     let mode = this.modesTemp.find(mode => { return mode.mode === selectedMode });
            //     mode.parameters.forEach(param => {
            //         this.$refs[`${mode.ref}-${param.name}`][0].restoreParam();
            //     });
            // }
        },
        // Restore all params to default
        restoreAllPreviousValues() {
            this.$emit('restoreValueFromMode', '');
            // this.modes.forEach(mode => {
            //     mode.parameters.forEach(param => {
            //         this.$refs[`${mode.mode}-${param.name}`][0].restoreParam();
            //     })
            // });

            // this.modesTemp.forEach(mode => {
            //     mode.parameters.forEach(param => {
            //         this.$refs[`${mode.ref}-${param.name}`][0].restoreParam();
            //     })
            // });
        },
        // Restores params to initial states values from not selected modes
        restoreToInitialDefault() {
            this.$emit('restoreToInitialDefault', this.itemModeSelected);
            // this.modes.filter((mode) => {return mode.mode != this.itemModeSelected}).forEach(mode => { 
            //     mode.parameters.forEach(param => {
            //         param.value = param.default;
            //         this.$refs[`${mode.mode}-${param.name}`][0].setCurrentDefault(param.default);
            //     });
            // });

            // this.modesTemp.filter((mode) => {return mode.mode != this.itemModeSelected}).forEach(mode => { 
            //     mode.parameters.forEach(param => {
            //         param.value = param.default;
            //         this.$refs[`${mode.ref}-${param.name}`][0].setCurrentDefault(param.default);
            //     });
            // });
        }
    }
})
