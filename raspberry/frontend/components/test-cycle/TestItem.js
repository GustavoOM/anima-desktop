Vue.component('test-item', {
    props: {
        description: '',
        state: ''
    },
    template: `
        <li>
            <img class="icon-check" style="visibility:hidden;" v-if="state === 'waiting'" src="./images/check-bold.svg" alt="success">
            <span class="icon-loader" v-if="state === 'running'"></span>
            <img class="icon-check" style="display:hidden;" v-if="state === 'check'" src="./images/check-bold.svg" alt="success">
            <img class="icon-error" v-if="state === 'error'" src="./images/alert-circle-red.svg" alt="error">
            <span class="description">
                {{ description }}
            </span>
        </li>
    `
})