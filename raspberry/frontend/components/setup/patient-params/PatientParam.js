Vue.component('patient-param-input', {
    props: {
        parameter: {},
        paramSelected: {},

        limitLabel: '', // should be 'min' or 'max'
    },
    data: function () {
        return {
            originalParamValue: "",
            paramValue: 0
        }
    },
    created: function () {
        this.originalParamValue = this.parameter.value;
        this.paramValue = this.parameter.value;
    },
    template: `
        <div class="param-input" v-bind:class="this.parameter === this.paramSelected ? '-selected' : '' ">
            <div class="steps-buttons" v-if="this.parameter === this.paramSelected">
                <div class="down-btn" v-on:click="decrement">-</div>
                <div class="up-btn" v-on:click="increment">+</div>
            </div>

            <div class="control" v-bind:class="this.parameter === this.paramSelected ? '-selected' : '' ">
                
                <input class="value" type="number" v-bind:ref="parameter.name"
                     :min="parameter.range[0]" 
                     :max="parameter.range[1]"
                     :step="parameter.step"
                     v-on:click="selectParameter()" 
                     v-model="parameter.value"/>

                <div class="unit">{{parameter.unit}}</div>


                <div class="title">
                    <span v-bind:class="{ '-fix-paramater-name-size': limitLabel }" >
                        {{parameter.name}}
                    </span>
                    <span v-if="limitLabel == 'min'" class="min-max">-MIN-</span>
                    <span v-if="limitLabel == 'max'" class="min-max">-MAX-</span>
                </div>

            </div>
        </div>
    `,
    methods: {
        selectParameter() {
            this.$parent.$emit('parameterSelected', this.parameter);
        },
        increment() {
            let value = Number(this.parameter.value);

            // do nothing if value has reached MAX
            if (value === this.parameter.range[1]) return;

            // increment
            value = this.add(value, this.parameter.step, 2);
            this.parameter.value = value.toString();
            this.$refs[this.parameter.name].focus();
        },
        decrement() {
            let value = Number(this.parameter.value);

            // do nothing if value has reached MIN
            if (value === this.parameter.range[0]) return;

            // decrement
            value = this.add(value, (this.parameter.step) * (-1), 2);
            this.parameter.value = value.toString();
            this.$refs[this.parameter.name].focus();
        },
        add(a, b, precision) {
            var x = Math.pow(10, precision || 2);
            return (Math.round(a * x) + Math.round(b * x)) / x;
        },
        
    }
})