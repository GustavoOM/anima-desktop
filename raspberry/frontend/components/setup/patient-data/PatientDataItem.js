Vue.component('patient-data-item', {
    props: {
        description: '',
        valueLabel: '',
        iconLabel: ''
        
    },
    template: `

        <div class="item">
            <p class="description">{{ description }}:</p>
            <div class="value" v-bind:class="{ '-center': iconLabel }">
                <span>{{ valueLabel }}</span>
                <img v-if="iconLabel" v-bind:src="getImageFromIconLabel()" v-bind:alt="description">
            </div>
        </div>
        
    `,
    methods: {
        getImageFromIconLabel() {
            return `./images/icon-${this.iconLabel}-dark.svg`;
        }
    }
})