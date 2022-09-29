Vue.component('header-setup', {
    props: {
        hourmeter: '', 
        messages: [],
        powerIcon: '',
        batteryIcon: ''
    },
    data: function () {
        return { }
    },
    template: `
        <!-- Header -->
        <div class="header">

            <div class="logo">
                <img src="./images/logo_exigido-2.png" alt="">
            </div>

            <!-- Alerts -->
            <div class="alerts">
                <!-- alert-pane v-bind:messages="messages"></alert-pane -->
            </div>

            <!-- Clock and power -->
            <clock-pane v-bind:powerIcon="powerIcon" v-bind:batteryIcon="batteryIcon" v-bind:hourmeter="hourmeter"></clock-pane>
        </div>
    `,
})
