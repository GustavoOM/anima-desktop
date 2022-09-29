Vue.component('patient-params', {
    props: {
        
    },
    data: function() {
        return {
            patientParams: this.$parent.patientParams,
            patientData: this.$parent.patientData,
            patientType: this.$parent.patientType,

            someSelected: false,
            showContinueTask: {},
            paramSelected: {}
        }
    },
    created: function() {
        this.listenRequiredEvents();
    },
    template: `

    <div class="patient-params">
        <div class="content -start">
            <div class="title">Novo Paciente</div>

            <patient-data-view v-bind:patient-data="patientData" v-bind:mode="patientParams.mode"/>

            <div class="title">Parâmetros</div>

            <div id="paramWrapper" class="data">
                
                <shape-options 
                    v-if="patientParams.graphShape"
                    v-model="patientParams.graphShape"
                />
                
                <patient-param-input
                    v-for="param in patientParams.parameters" 
                    v-bind:parameter="param"
                    v-bind:paramSelected="paramSelected"
                /> 

            </div>
        </div>
        <div class="btn-group">
            <button id="back1" class="btn -cancel" v-on:click="goBackPatientData">Voltar</button>
            <button id="continue2" class="btn -continue" v-on:click="goToAlarms" v-visible="!someSelected">Continuar</button>
        </div>
    </div>
        
    `,
    methods: {
        listenRequiredEvents() {
            this.$on('parameterSelected', paramSelected => {
                this.paramSelected = paramSelected;
            });
        },
        goBackPatientData() {
            let paramsChanged = 
                this.verifyForParametersChanges();
                
            // if(paramsChanged) {
            //     let proceed = confirm("As alterações de parâmetros serão perdidas. Deseja continuar?");
            //     if(!proceed) return
            // }

            this.$parent.currentStep = "patientDataStep";
        },
        goToAlarms() {
            this.$parent.patientParams = this.patientParams;
            this.$parent.currentStep = "patientAlarmsStep";
        },
        verifyForParametersChanges() {

            let params = this.patientParams.parameters;
            let gShape = this.patientParams.graphShape;

            var shapeChanged = gShape && gShape.value != gShape.default;
            var paramsChanged = !params.every(p => parseFloat(p.value) == p.default);
            
            return shapeChanged || paramsChanged ;
        }
    }
})
