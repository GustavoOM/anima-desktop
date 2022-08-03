
Vue.component('sensor-item', {
    props: {
        sensor: {},
        sensorGroup: [],
        mode: ''
    },
    template: `<div class="item" v-bind:class="{ 'sensor-alert': sensor.alert || hasAnyAlert() }">
                    <p class="title">
                        {{ sensor.name}}
                        <span class="unit">{{sensor.unit}}</span>
                    </p>
                    <div class="value">
                        {{ formatValue(sensor.value) }}
                    </div>

                    <div class="small-sensor" v-for="s in sensorGroup">
                        <p class="title">{{s.value.name}}:
                        <span class="value">{{ formatValue(s.value.value) }} {{s.value.unit}}</span>
                        </p>
                    </div>
                </div>`,
    methods: {
        hasAnyAlert() {
            let has = false;
            this.sensorGroup.forEach(s => { 
                if(s.value.alert)
                    has = true;
            })
            return has;
        },
        formatValue(value) {
            if(typeof value === 'string' && value.trim() !== "---") {
                let formatted = Number(value);
                // remove second decimal
                formatted = Math.floor(formatted * 10) / 10;
                value = formatted.toString();
            } else if(typeof value === 'number') {
                value = Math.floor(value * 10) / 10;
            }
            return value;
        }
    }
});

// `<div class="item" v-bind:class="{ 'sensor-alert': sensor.alert }">
//                     <p class="title">{{ sensor.name}}</p>
//                     <div class="value">
//                         <span>{{ !sensor.alert ? sensor.value : sensor.alert.value}}</span>
//                         <span class="unit">{{sensor.unit}}</span>
//                     </div>
//                     <div class="data-alert">
//                         <p class="max">{{sensor.max}}</p>
//                         <p class="min">{{sensor.min}}</p>
//                     </div>
//                 </div>`

