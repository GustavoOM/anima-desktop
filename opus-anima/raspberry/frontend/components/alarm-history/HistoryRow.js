Vue.component('history-row', {
    props: {
        date: '',
        time: '',
        duration: '',
        name: '',
        value: '',
        id: 0
    },
    template: `
        <tr v-on:click="getInfo">
            <td>{{ date }}</td>
            <td>{{ time }}</td>
            <td>{{ duration }}</td>
            <td>{{ name }}</td>
            <td>{{ value }}</td>
        </tr>
    `,
    methods: {
        getInfo() {
            this.$parent.$emit("getAlarmInfo", this.id);
        }
    }
})