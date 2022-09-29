Vue.component('patient-data-view', {
    props: {
        mode: '',
        patientData: ''
    },
    data: function() {
        return {}
    },
    created: function() {
    },
    template: `

        <div class="data">
            <patient-data-item 
                description="Nome" 
                v-bind:value-label=" patientData.name ? patientData.name : '-' "
            />
            <patient-data-item 
                description="Prontuário" 
                v-bind:value-label=" patientData.prontuary ? patientData.prontuary : '-' "
            />
            <patient-data-item 
                description="Paciente" 
                v-bind:value-label="mapAgeGroupToLabel(patientData.age)" 
                v-bind:icon-label="patientData.age"
            />
            <patient-data-item 
                description="Sexo" 
                v-bind:value-label="mapGenderValueToLabel(patientData.gender)"
            />
            <div style="display: flex; justify-content: space-between;">
                <patient-data-item 
                    description="Altura" 
                    v-bind:value-label="formatNumericalValue(patientData.height, 'cm')"
                />
                <patient-data-item 
                    description="Peso" 
                    v-bind:value-label="formatNumericalValue(patientData.weight, 'Kg')"
                />
            <div>
        </div>
        
    `,
    methods: {
        goBackNewPatient() {
            this.$parent.currentStep = "patientTypeStep";
        },
        goToPatientParams() {
            this.$parent.patientData = this.patientData;
            this.$parent.currentStep = "patientParamsStep";
        }
    }
})