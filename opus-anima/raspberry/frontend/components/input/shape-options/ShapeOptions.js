Vue.component('shape-options', {
    props: { value: {} },

    created: function () {
        this.populateShapeOptions();
        this.listenShapeChanged();
    },
    data: function () {
        return {
            shapes: []
        }
    },
    template: `

        <div class="curves" >
            <shape-option-item v-for="shape in shapes" v-bind:shape="shape" />
        </div>

    `,
    methods: {
        populateShapeOptions() {
            // Get shape options
            this.shapes = this.getShapeOptions();

            // Get component shapeModel 
            let shapeModel = this.value;

            // Get shapeOption from shape options, based on shapeModel
            let shapeOption = this.shapes
                .find(shape => shape.value == shapeModel.value);
            
            // Set obtained shapeOption selected
            shapeOption.selected = true;
        },
        listenShapeChanged() {
            this.$on('shapeChanged', shape => {
                this.$emit('input', shape);
                this.$parent.$emit('applyParameter', shape);
            });
        }
    }
})