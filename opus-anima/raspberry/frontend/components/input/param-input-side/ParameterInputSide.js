Vue.component('param-input-side', {
    props: {
        parameter: {},
        mode: '',
        selectedParam: {},
        isAlarm: false,
        limitLabel: '', // should be 'min' or 'max'
    },
    data: function () {
        return {
            viewValue: 0,
            // New default value after a mode selection
            currentDefault: "",
            mapBkMode: {
                1: 'VCV',
                2: 'PCV'
            }
        }
    },
    created: function () {
        this.listenRequiredEvents();

        if(this.isAlarm) {
            this.listenUpdateDefaultAlarmBounds();
            this.listenResetAlarmBounds();
        }

        this.currentDefault = this.parameter.value;
        if(this.isAlarm && this.parameter.value == 0)
            this.viewValue = 'OFF';
        else {
            // console.log("Updating " + this.parameter.id + ": " + this.viewValue + " = " + this.parameter.value)
            this.viewValue = this.parameter.value;
        }
    },
    template: `
        <div class="param-input-side" v-bind:class="isAlarm ? '-alarm' : '' ">

            <div class="title">
                <span>{{parameter.name}}</span>
                <span v-if="limitLabel == 'min'" class="min-max">-MIN-</span>
                <span v-if="limitLabel == 'max'" class="min-max">-MAX-</span>
            </div>
                
            <div class="info">
                <div class="unit" v-if="parameter.id === 'bkMode'">{{ mapBkMode[parameter.value] }}</div>
                <div class="unit" v-else>{{parameter.unit}}</div>
                <input class="value" type="text" v-bind:ref="parameter.name" v-on:keydown="keyboardInput(event)"
                     v-on:click="selectParameter()" 
                     v-model="viewValue"
                     v-bind:class="parameter === selectedParam ? '-selected' : '' "
                     />
            </div>

            <div class="steps-buttons" v-if="parameter === selectedParam">
                <div class="up-btn" v-on:click="applyStepTouch(1)"></div>
                <div class="down-btn" v-on:click="applyStepTouch(-1)"></div>
            </div>

        </div>
    `,
    methods: {
        selectParameter() {
            this.$parent.$emit('selection', this.parameter);
        },
        listenRequiredEvents() {
            this.$parent.$on('updateValuesFromMode', (selectedMode, parameter = {}) => {
                if(selectedMode === this.mode) {
                    if(parameter && parameter.name === this.parameter.name) this.setCurrentDefault(parameter.value);
                    else this.setCurrentDefault();
                }
            });
            this.$parent.$on('restoreValueFromMode', (selectedMode) => {
                if(selectedMode === this.mode || selectedMode === '') this.resetValue()
            });
            this.$parent.$on('restoreToInitialDefault', (selectedMode) => {
                if(selectedMode !== this.mode) this.setCurrentDefault(this.parameter.default);
            });
        },
        listenUpdateDefaultAlarmBounds() {
            this.$parent.$on('updateDefaultAlarmBounds', () => { this.setCurrentDefault(); });
        },
        listenResetAlarmBounds() {
            this.$parent.$on('resetAlarmBounds', () => { this.resetValue(); });
        },
        keyboardInput(event) {
            event.preventDefault();
            if(event.key === "ArrowUp")
                this.applyStepKey(1)
            if(event.key === "ArrowDown")
                this.applyStepKey(-1)
        },
        applyStepTouch(op) {
            let value = Number(this.parameter.value);
            value = this.add(value, this.parameter.step * Math.sign(op), 2);

            if (value >= this.parameter.range[1]) 
                value = this.parameter.range[1];
            else if (this.isAlarm)
                value = this.checkMinAlarmRange(value, op);
            else if (value <= this.parameter.range[0]) 
                value = this.parameter.range[0];

            this.parameter.value = value.toString();

            if(this.isAlarm && value == 0)
                this.viewValue = 'OFF';
            else 
                this.viewValue = this.parameter.value;

            this.$refs[this.parameter.name].focus();
        },
        applyStepKey(op) {
            let value = Number(this.parameter.value);
            value = this.add(value, this.parameter.step * Math.sign(op), 2);
            
            if (value >= this.parameter.range[1]) 
                value = this.parameter.range[1];
            else if (this.isAlarm)
                value = this.checkMinAlarmRange(value, op);
            else if (value <= this.parameter.range[0]) 
                value = this.parameter.range[0];

            this.parameter.value = value.toString();

            if(this.isAlarm && value == 0)
                this.viewValue = 'OFF';
            else 
                this.viewValue = this.parameter.value;

            this.$refs[this.parameter.name].focus();
        },
        add(a, b, precision) {
            var x = Math.pow(10, precision || 2);
            return (Math.round(a * x) + Math.round(b * x)) / x;
        },
        checkMinAlarmRange(value, op) {
            if (op === 1 && this.parameter.value == 0) value = this.parameter.range[0];
            else if (value < this.parameter.range[0]) value = 0;
            return value;
        },
        // Set new Default value
        setCurrentDefault(value) {
            if(value) this.parameter.value = value;
            this.currentDefault = this.parameter.value;

            if(this.isAlarm && this.parameter.value == 0) this.viewValue = 'OFF';
            else this.viewValue = this.parameter.value;
        },
        resetValue() {
            
            this.parameter.value = this.currentDefault;

            if(this.isAlarm && this.parameter.value == 0) this.viewValue = 'OFF';
            else this.viewValue = this.parameter.value;
        }
    },
})
