Vue.component('shape-option-item', {
    props: {
        shape: {},
    },
    data: function () {
        return {
            selected: false
        }
    },
    template: `

        <div class="curve" v-bind:class="{'-selected': selected }" v-on:click="curveSelected">
            <img v-bind:src="'./images/curve'+ shape.value + '.svg'" v-bind:alt="shape.type">
        </div>

    `,
    created: function () {
        this.selected = this.shape.selected;
        this.listenOtherShapeSelected();
    },
    methods: {
        curveSelected() {
            this.triggerShapeSelected();
            this.setSelected(true);
        },
        listenOtherShapeSelected() {
            this.$parent.$on('shapeSelected', shapeValue => {
                
                let otherSelected = this.shape.value != shapeValue
                let notSelectedBefore = !this.selected;

                if(otherSelected)
                    this.setSelected(false);

                else if(notSelectedBefore)
                    this.triggerShapeChanged();
            });
        },
        triggerShapeSelected() {
            this.$parent.$emit('shapeSelected', this.shape.value);
        },
        triggerShapeChanged() {
            this.$parent.$emit('shapeChanged', this.shape);
        },
        setSelected(selected) {
            this.shape.selected = this.selected = selected;
        }
    },
})