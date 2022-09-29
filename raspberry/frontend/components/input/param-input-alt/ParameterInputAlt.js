Vue.component('param-input-alt', {
    props: {
        parameter: {},

        limitLabel: '', // should be 'min' or 'max'
        applyValueOnChange: false,
        enableAcceptControls: false,
        id: -1
    },
    data: function () {
        return {
            editable: false,
            previousValue: undefined,
            lastClickedElement: null,
            inputClickEvent: null,
            finishingEdit: false,
            showStepButtons: false,
            selectedIndex: 1
        }
    },
    template: `
        <div class="param-input-alt" 
            v-bind:class="{'-selected': editable, '-alt': limitLabel, '-black-border': enableAcceptControls && editable, [parameter.id]: parameter.id  }"  
            v-on:click="parameterSelected" >

            <div class="no-controls" v-if="!(enableAcceptControls && editable)">

                <div class="title-alt">
                    <span v-bind:class="{ '-fix-paramater-name-size': limitLabel }" >
                        {{parameter.name}}
                    </span>
                    <span v-if="limitLabel == 'min'" class="min-max">-MIN-</span>
                    <span v-if="limitLabel == 'max'" class="min-max">-MAX-</span>
                </div>
                
                <div class="info-alt">
                    <div class="unit">{{parameter.unit}}</div>
                    <div class="value" v-show="!editable">{{ parameter.value }}</div>
                    <input type="number" id="inner-input-alt" ref="parameterInput" 
                        v-show="editable"
                        :min="parameter.range[0]" 
                        :max="parameter.range[1]"
                        :step="parameter.step"
                        :value="parameter.value"
                        v-model="parameter.value"
                        v-on:blur="disableEdition"
                        v-on:input="setPrecision"
                        v-on:focus="setPrecision" 
                    />
                </div>
            </div>

            <div class="with-controls" v-if="enableAcceptControls && editable">
                <div>
                    <span class="title" >{{parameter.name}}</span>
                    <span v-if="limitLabel == 'min'" class="min-max">-MIN-</span>
                    <span v-if="limitLabel == 'max'" class="min-max">-MAX-</span>
                </div>
                <div class="input-unit">
                    <input type="number" class="parameter-input" ref="parameterInput"
                        v-show="editable"
                        :min="parameter.range[0]" 
                        :max="parameter.range[1]"
                        :step="parameter.step"
                        :value="parameter.value"
                        v-model="parameter.value"
                        v-on:blur="disableEdition"
                        v-on:input="setPrecision"
                        v-on:focus="setPrecision" 
                    />
                    <div class="parameter-unit" >{{parameter.unit}}</div>
                </div>
            </div>

            <div v-if="enableAcceptControls && editable" class="accept-controls" >
                <button class="cancel-button" v-on:click="cancelEdition">
                    <span>X</span>
                </button>
                <button class="confirm-button" v-on:click="applyInputedValue">
                    <span>&#x2714;</span>
                </button>
            </div>

            <div class="steps-buttons" v-if="showStepButtons">
                <div class="up-btn" v-on:click="increment"></div>
                <div class="down-btn" v-on:click="decrement"></div>
            </div>
        </div>
    `,
    created: function () {
        
        document.addEventListener('mousedown', (e) => {
            this.lastClickedElement = e.target;
        });

        document.addEventListener('mouseup', (e) => {
            this.lastClickedElement = null;
        });

    },
    methods: {
        parameterSelected(event) {

            // These logs will help a lot to debug this component
            // console.log(this.parameter.id)
            // console.log(Object.assign({},this._data))

            this.inputClickEvent = event;
            
            if(!this.previousValue)
                this.previousValue = this.parameter.value;  
            
            this.enableAcceptControls ?
                this.handleWithControlsSelected() :
                this.handleDefaultSelected();

            if(this.editable) {
                this.triggerParameterSelected();
                this.showStepButtons = true;
                this.$parent.$emit('showedStepButtons', this.id);
            }
            
        },

        disableEdition(event) {

            this.inputClickEvent = event;

            this.enableAcceptControls ?
                this.handleWithControlsEdition() :
                this.handleDefaultEdition();

        },

        handleDefaultSelected() {
            this.editable = !this.editable;
        },

        handleWithControlsSelected() {
            let noEditing = 
                !this.editable && !this.finishingEdit;

            if(noEditing) 
                this.editable = true;
            
            this.finishingEdit = false;
        },

        handleDefaultEdition() {
            if(this.notClickedAnyParameter() && this.notClickedInput()) 
                this.parameter.value = this.previousValue;

            else if(this.applyValueOnChange) 
                this.applyInputedValue();
            else
                this.finishEdition();
            
            this.triggerParameterUnselected();
        },

        handleWithControlsEdition() {
            if(this.notClickedThisParameter()) {
                this.parameter.value = this.previousValue;
                this.triggerParameterUnselected();
            }
        },

        cancelEdition() {
            this.parameter.value = this.previousValue;
            this.finishEdition();
        },

        applyInputedValue() {
            if(this.parameter.value != this.previousValue) 
                this.triggerApplyParameter()
            this.finishEdition();
        },

        applyInputedValueFromStepButton() {
            if(this.parameter.value != this.previousValue) 
                this.triggerApplyParameter()
            this.finishingEdit = true;
            this.editable = false;
        },

        finishEdition() {
            this.previousValue = undefined;
            this.finishingEdit = true;
            this.editable = false;
        },

        setPrecision(event) {
            event.target.value = parseFloat(event.target.value).toFixed(this.parameter.precision);
            this.parameter.value = event.target.value;
        },

        triggerParameterSelected() {
            setTimeout(() => { 
                this.$parent.$emit('parameterSelected', true);
                this.$refs.parameterInput.focus(); 
            }, 100);
        },

        triggerParameterUnselected() {
            setTimeout(() => {
                this.editable = false;
                this.$parent.$emit('parameterSelected', false);
            }, 25);
        },

        triggerApplyParameter() {
            this.$parent.$emit('applyParameter', this.parameter);
        },

        notClickedAnyParameter() {

            let clickedShapeOptions = this.clickedShapeOptions();
            let clickedParameter = this.clickedParameter();

            return !(clickedShapeOptions || clickedParameter);
        },

        notClickedThisParameter() {

            let parameterId = this.parameter.id;
            let clickedThisInput = this.clickedThisInput(parameterId);
            return !clickedThisInput;
        },

        notClickedInput() {

            let notLastClickedElement = !this.lastClickedElement;
            let clickedInnerInput = this.clickedInnerInput();

            return !(clickedInnerInput && notLastClickedElement);
        },
        
        clickedShapeOptions() {
            return this.lastClickedElement && this.lastClickedElement.closest('div.curve');
        },

        clickedParameter() {
            return this.lastClickedElement && this.lastClickedElement.closest('div.param-input');
        },

        clickedThisInput(parameterId) {
            return this.lastClickedElement && this.lastClickedElement.closest(`div.param-input.${parameterId}`);
        },

        clickedInnerInput() {
            return this.inputClickEvent && this.inputClickEvent.target.id == 'inner-input';
        },
        increment() {
            let value = parseInt(this.parameter.value) + this.parameter.step;
            this.parameter.value = value.toString();
            this.applyInputedValue();
        },
        decrement() {
            let value = parseInt(this.parameter.value) - this.parameter.step;
            this.parameter.value = value.toString();
            this.applyInputedValue();
        },
        hideStepButtons() {
            this.showStepButtons = false;
        }
    }

})