Vue.component('param-input', {
    props: {
        parameter: {},
        selectedFooterParam: {},
        indicate: false,
        limitLabel: '', // should be 'min' or 'max'
    },
    data: function () {
        return {
            originalParamValue: "",
            isFiveDigitValue: false
        }
    },
    created: function () {
        this.originalParamValue = this.parameter.value;
        if(this.parameter.value.length >= 4)
            this.isFiveDigitValue = true;
    },
    template: `
        <div class="param-input" v-bind:class="this.parameter === this.selectedFooterParam ? '-selected' : '' ">
            
            <div class="steps-buttons" v-if="this.parameter === this.selectedFooterParam">
                <div class="down-btn" v-on:click="applyStepTouch(-1)">-</div>
                <div class="up-btn" v-on:click="applyStepTouch(1)">+</div>
            </div>

            <div class="control" v-bind:class="this.parameter === this.selectedFooterParam ? '-selected' : '' ">
                <div class="small-title" v-if="this.parameter === this.selectedFooterParam">
                    <span>{{parameter.name}}</span>
                </div>
                
                <input class="value" type="number" v-bind:ref="parameter.name" v-on:blur="keepFocus()" v-on:keydown="keyboardInput(event)"
                    v-on:click="confirmParameter()" v-bind:class="{'-five-digit': isFiveDigitValue, '-selected': this.parameter === this.selectedParam }"
                     :min="parameter.range[0]" 
                     :max="parameter.range[1]"
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

            <div v-if="this.parameter === this.selectedFooterParam" class="accept-controls" >

                <button class="cancel-button" v-on:click="cancelParameter()">
                    <span>X</span>
                </button>

                <button class="confirm-button" v-on:click="confirmParameter()">
                    <span>&#x2714;</span>
                </button>

            </div>

        </div>
    `,
    methods: {
        keepFocus() {
            if(this.parameter === this.selectedFooterParam){
                this.$refs[this.parameter.name].focus();
            }
        },
        keyboardInput(event) {
            event.preventDefault();
            if(event.key === "ArrowUp")
                this.applyStepKey(1)
            if(event.key === "ArrowDown")
                this.applyStepKey(-1)
        },
        cancelParameter() {
            this.$parent.$emit('resetParameter', this.parameter);
            this.parameter.value = this.originalParamValue;
        },
        confirmParameter() {
            if (this.parameter === this.selectedFooterParam) {
                this.$parent.$emit('applyParameter', this.parameter);
                this.originalParamValue = this.parameter.value;
            }
        },
        selectParameter() {
            if (this.parameter !== this.selectedFooterParam) {
                this.$parent.$emit('selection', this.parameter);
                this.$refs[this.parameter.name].focus();
            }
        },
        applyStepTouch(op) {
            let value = Number(this.parameter.value);
            value = this.add(value, this.parameter.step * Math.sign(op), 2);

            if (value >= this.parameter.range[1]) 
                value = this.parameter.range[1];
            else if (value <= this.parameter.range[0]) 
                value = this.parameter.range[0];

            this.parameter.value = value.toString();

            this.$refs[this.parameter.name].focus();
        },
        applyStepKey(op) {
            let value = Number(this.parameter.value);
            value = this.add(value, this.parameter.step * Math.sign(op), 2);

            if (value >= this.parameter.range[1])
                value = this.parameter.range[1];
            else if (value <= this.parameter.range[0]) 
                value = this.parameter.range[0];

            this.parameter.value = value.toString();

            this.$refs[this.parameter.name].focus();
        },
        add(a, b, precision) {
            var x = Math.pow(10, precision || 2);
            return (Math.round(a * x) + Math.round(b * x)) / x;
        },
        
    }
})
