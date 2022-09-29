class SocketService {

    constructor() {
        this._socketIo = 
            !appSettings.simulateSocketIo 
                ? io.connect(appSettings.monitorSocketBaseUrl)
                : { on: () => {} } 
    }

    registerEvent(eventName, callback) {
        this._socketIo.on(eventName, function (data) {
            callback(data);
        });
    }
}
