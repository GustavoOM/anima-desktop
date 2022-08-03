/*
============================================================================
== SERVICES DEPENDENCIES ==                                               ==
============================================================================ */
Vue.mixin({
    // Creates instances of desired services.
    // This behaviour is for to implement a kind of 'dependency injection'.
    //   To inject a service in a component, attach the desired service placed 
    //   on $root to some property in the 'beforeCreate' component event.
    beforeMount: function() {

        // Monitor service
        this.$root.$monitorService = 
            this.$root.$monitorService || new MonitorService();
        
        // Session service
        this.$root.$sessionService = 
            this.$root.$sessionService || new SessionService();
    }
});

/* 
============================================================================
== SETUP COMPONENT ==                                                     ==
== Defines all steps to create a configuration for the patient.           ==
== After setup completes, it goes to monitor screen and starts monitoring ==
============================================================================ */
Vue.component('index', {
    
    // Componente data
    data: function() {
        return {
            patientData: {
                age: '',
                gender: '',
                height: 0,
                weight: 0,
                name: '',
                prontuary: ''
            },
            patientParams: {
                mode: null,
                graphShape: '',
                parameters: []
            },
            patientAlarms: {
                minValues: [],
                maxValues: [],
                othersValues: []
            },
            hourmeter: '',
        }
    },

    // Service injections
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
        this.$sessionService = this.$root.$sessionService;
    },

    // Component created event
    created: function() {
        this.$monitorService.getStatus( response =>{
            
            this.hourmeter = response.hourmeter;

            if (!response.restored){
                this.goToTest();
            };

            this.patientData.age = response.patient.age_group;
            this.patientData.gender = response.patient.sex;
            this.patientData.height = response.patient.height;
            this.patientData.weight = response.patient.weight;
            this.patientData.name = response.patient.name;
            this.patientData.prontuary = response.patient.prontuary;
            this.patientParams.mode = response.strategy.mode;
    
            this.getSettings(settings => {
                
                this.setModeParameters(settings);
                this.setModeAlarms(settings);  

                if (response.strategy.shape){
                    this.patientParams.graphShape.value = response.strategy.shape;
                };
                this.patientParams.parameters.forEach(param => {
                    param.value = response.strategy[param.id];
                });
        
                this.patientAlarms.minValues.forEach(alarm => {
                    alarm.value = response.boundaries[alarm.id];
                });
                this.patientAlarms.maxValues.forEach(alarm => {
                    alarm.value = response.boundaries[alarm.id];
                });
                this.patientAlarms.othersValues.forEach(alarm => {
                    alarm.value = response.boundaries[alarm.id];
                });
        
                this.goToMonitor();
            });
        });
    },

    // HTML template
    template: `
        <div>
            <p> Index </p>
        </div>
    `,

    // Component methods
    methods: {
        getSettings(callback){ 
            this.$monitorService.getSettings(callback);     
        },
        setModeParameters(settings) {
            let modeConfiguration = settings.modes
                .find(mode => mode.mode == this.patientParams.mode).configuration

            this.patientParams.graphShape = modeConfiguration
                .find(config => config.id == 'shape');

            this.patientParams.parameters = modeConfiguration
                .filter(config => config.id != 'shape');
        },
        setModeAlarms(settings) {

            let modeAlarms = settings.alarms

            this.patientAlarms.minValues = modeAlarms.min
            this.patientAlarms.maxValues = modeAlarms.max

            this.patientAlarms.othersValues = modeAlarms.others;
        },
        goToMonitor(){
            this.$sessionService.set('setupConfiguration', { 
                mode: { 
                    patient: this.patientData,
                    mode: this.patientParams.mode,
                    graphShape: this.patientParams.graphShape,
                    parameters: this.patientParams.parameters
                },
                alarms: this.patientAlarms,
            });
            this.$sessionService.set('hourmeter', this.hourmeter);
            document.location = './monitor.html';
        },
        goToTest() {
            this.$sessionService.set('hourmeter', this.hourmeter);
            document.location = './test-cycle.html';
            
        }
    }

});
