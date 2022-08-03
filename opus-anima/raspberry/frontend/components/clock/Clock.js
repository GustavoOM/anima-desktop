
Vue.component('clock-pane', {
    props: {
        hourmeter: '',
        powerIcon: '',
        batteryIcon: ''
    },
    data: function() {
        return {
            currentDate : '',
            currentTime : '',
            initialTime: '',
            initialDate: '',
        }
    },
    template: `<div class="system">
                    <div class="upper-clock">
                    <div class="icons">
                        <img v-bind:src="powerIcon" class="power-supply" alt="Power supply indicator">
                        <img v-bind:src="batteryIcon" class="power-supply" alt="Power supply indicator">
                    </div>
                        <div class="clock">
                            <p>{{ currentTime }}</p>
                            <p>{{ currentDate }}</p>
                        </div>
                    </div>
                    <div class="bottom-clock">
                        <div class="clock">
                            <p class="clock-item">{{ initialTime }}</p>
                            <p class="clock-item">{{ initialDate }}</p>
                            <p class="clock-item">{{ hourmeter }}h</p>
                        </div>
                    </div>
                </div>`,
    created:function(){
        var { time, date } = this.getCurrentTimestamp();

        this.initialTime = time;
        this.initialDate = date;
        
        setInterval(this.updateCurrentTimestamp, 1000);
    },
    methods: {
        updateCurrentTimestamp() {
            var { time, date } = this.getCurrentTimestamp();

            this.currentTime = time;
            this.currentDate = date;
        },

        getCurrentTimestamp(){
            var today = new Date();
            
            return { time: this.getDate(today), date: this.getTime(today) };
        },
        getDate(today) {
            var day = this.formatNumberTwoDigits(today.getDate());
            var month = this.formatNumberTwoDigits((today.getMonth()+1));
            var year = today.getFullYear()

            return `${day}/${month}/${year}`;
        },
        getTime(today){
            var hour = this.formatNumberTwoDigits(today.getHours());
            var minute = this.formatNumberTwoDigits(today.getMinutes());
            var second = this.formatNumberTwoDigits(today.getSeconds());

            return `${hour}:${minute}:${second}`;
        },
        formatNumberTwoDigits(number){
            return number.toString().padStart(2,"0")
        }
    }

});