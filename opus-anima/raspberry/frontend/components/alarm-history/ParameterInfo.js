Vue.component('parameter-info', {
    props: {
        name: '',
        value: '',
        unit: ''
    },
    template: `
        <div class="item">
            <div class="title">{{ name }}</div>
            <div class="value">{{ value }}</div>
            <div class="unit">{{ unit }}</div>
        </div>
    `
})