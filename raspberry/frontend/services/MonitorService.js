class MonitorService {

    constructor() {
        this._baseUrl = appSettings.monitorApiBaseUrl;
    }
    
    // General Operations
    getLastAlarms(num_alarms, callback) {
        this._get('/log/list-' + num_alarms).then(response => 
            this._handleCallback(response, callback)
        );
    }
    getAlarmInfo(alarmId, callback) {
        this._get('/log/info-' + alarmId).then(response => 
            this._handleCallback(response, callback)
        );
    }
    getTestData(callback) {
        this._get('/test/info').then(response => 
            this._handleCallback(response, callback)
        );
    }
    startTest(testValue, callback) {
        this._post('/test/start', { test: testValue }).then(response => 
            this._handleCallback(response, callback)
        );
    }
    resetTest(cycleValue, callback) {
        this._post('/test/reset', {"cycle": cycleValue}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    checkTest(testId, callback) {
        this._get('/test/check-' + testId).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendTestSound(callback) {
        this._post('/test/audio-start', {"audio": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendStartCalibration(callback) {
        this._post('/ex-flow-calib/start', {"exFlowCalib": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    checkCalibration(callback) {
        this._get('/ex-flow-calib/check').then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendStartCalibration(callback) {
        this._post('/ex-flow-calib/start', {"exFlowCalib": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    getModes(callback) {
        this._get('/modes').then(response => 
            this._handleCallback(response.modes, callback)
        );
    }
    getStatus(callback) {
        this._get('/status').then(response =>
            this._handleCallback(response, callback)
        );
    }
    getShutdown(){
        this._get('/turnoff');
    }
    setPatient(patient, callback) {
        patient.age_group = patient.age;
        patient.sex = patient.gender;
        this._post('/patient', this._normalizeNumbers(patient)).then(response => 
            this._handleCallback(response, callback)
        );
    }
    getSettings(callback) {
        this._get('/settings').then(response => {

            response.modes.forEach(m => 
                m.configuration = m.configuration
                    .map(this._extractValueAndPrecision)
            );


            response.alarms.min.forEach(alarm => {
                this._extractValueAndPrecision(alarm);
                alarm.name = alarm.name.replace(' [min]','');
            });

            response.alarms.max.forEach(alarm => {
                this._extractValueAndPrecision(alarm);
                alarm.name = alarm.name.replace(' [max]','');
            });


            // let othersAlarms = response.alarms.set.map(alarm => 
            //     this._extractValueAndPrecision(alarm)
            // );

            let hasMinMaxFromMin = 
                response.alarms.min.filter(alarm => response.alarms.max.some(a => a.name == alarm.name));

            let hasMinMaxFromMax = 
                response.alarms.max.filter(alarm => response.alarms.min.some(a => a.name == alarm.name));

            let othersFromMin = 
                response.alarms.min.filter(alarm => hasMinMaxFromMin.every(a => a.name != alarm.name));

            let othersFromMax = 
                response.alarms.max.filter(alarm => hasMinMaxFromMax.every(a => a.name != alarm.name));
                

            othersFromMin.forEach(alarm => alarm.limitLabel = 'min');
            othersFromMax.forEach(alarm => alarm.limitLabel = 'max');

            response.alarms.min = hasMinMaxFromMin;
            response.alarms.max = hasMinMaxFromMax

            response.alarms.others = []
                .concat(othersFromMin)
                .concat(othersFromMax);
                // .concat(othersAlarms);
            

            this._handleCallback(response, callback)
        });
    }
    sendConfiguration(config, callback) {

        let parameters = Object.assign([],config.parameters);

        if(config.graphShape)
            parameters = parameters.concat([config.graphShape]);

        let configuration = this._extractConfiguration(parameters);
    
        let modeConfiguration = {
            mode: config.mode,
            configuration: configuration
        }

        this._post('/configure', modeConfiguration).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendAlarms(alarms, callback) {

        let alarmValues = []
            .concat(alarms.minValues)
            .concat(alarms.maxValues)
            .concat(alarms.othersValues);

        let alarmsConfiguration = 
            this._extractConfiguration(alarmValues);

        let boundariesAlarms = {
            boundaries: alarmsConfiguration
        };

        this._post('/boundaries', boundariesAlarms).then(response => 
            this._handleCallback(response, callback)
        );

    }
    sendParameter(mode, parameter, callback) {

        let param = this._normalizeNumbers(parameter);

        let parameterToSend = {
            mode: mode,
            configuration: {
                [param.id]: param.value
            }
        }

        this._post("/configure", parameterToSend).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendStop(callback){
        this._post("/stop", {"stop": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendStart(callback){
        this._post("/start", {"start": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendSilence(callback){
        this._post("/silence", {"silence": true}).then(response => 
            this._handleCallback(response, callback)
        );
    }
    sendInitialParameters(callback) {
        this._post("/init-param", { }).then(response =>
            this._handleCallback(response, callback)
        );
    }
    setNightMode(nightmode, callback) {
        this._post("/night-mode", {"nightmode": nightmode}).then(response => 
            this._handleCallback(response, callback)
        );
    }

    // To simulate Socket IO messages
    async getGraphics(state) {
        const response = await this._get(`/graphics/${state}`, { enableLog: false });
        return response;
    }
    async getSensors(state) {
        const response = await this._get(`/sensors/${state}`, { enableLog: false });
        return response;
    }

    // Helper methods
    _extractValueAndPrecision(config) {``
        switch(config.id) {
            case 'shape':
                config.value = config.default;
                return config;
            default:
                config.precision = config.step.countDecimals();
                config.default = config.default.setPrecision(config.precision);
                config.value = config.default;
                return config;
        }
    }
    _extractConfiguration(valueObjects) {

        const configuration = valueObjects.reduce((config, obj) => {
            config[obj.id] = obj.value;
            return config; 
        },{})
        
        return this._normalizeNumbers(configuration);
    }
    _normalizeNumbers(toNormalize) {

        var obj = Object.assign({},toNormalize);

        const isNumeric = (value) => !isNaN(value);
        const isString = (value) => typeof value === 'string' || value instanceof String;

        for (let [key, value] of Object.entries(obj)) 
            if(isString(value) && isNumeric(value))
                obj[key] = value ? parseFloat(value) : null;

        return obj;
    }

    // HTTP operations
    _get(path, options = { enableLog: true }){

        if(options.enableLog)
            console.log(`Calling GET to ${path}`);

        console.log(`get from: ${this._baseUrl}${path}`);

        return fetch(`${this._baseUrl}${path}`)
                 .then(res => this._handleResponse(res, 'GET', options.enableLog))
                 .then(res => this._handleInterceptor(res, options.enableLog));

    }
    _post(url, data, options = { enableLog: true }){

        var dataToPost = Object.assign({},data)

        console.log(`Calling POST to ${url}. Posted data:`);
        console.table(dataToPost);

        return fetch(`${this._baseUrl}${url}`,{
            headers: {'Content-type' : 'application/json'},
            method: 'post',
            body: JSON.stringify(data)
        })
        .then(res => this._handleResponse(res,'POST', options.enableLog))
        .then(res => this._handleInterceptor(res, options.enableLog));
    }
   
    // Handlers of HTTP data
    _handleResponse(res, verb, enableLog){
        if(!res.ok) 
            throw new Error(res.statusText);
        
        if(enableLog)
            console.log(`Response HTTP-${res.status}-${res.statusText} from ${verb} to ${res.url}`);

        return res.json();
    }
    _handleInterceptor(response, enableLog) {
        if(enableLog)
            console.log(response);

        return response;
    }
    _handleCallback(result, callback) {
        if(callback)
            callback(result);
    }
}
