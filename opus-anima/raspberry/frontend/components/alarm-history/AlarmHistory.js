/*
============================================================================
== SERVICES DEPENDENCIES ==                                               ==
============================================================================ */
Vue.mixin({
    // Creates instances of desired services.
    // This behaviour is for to implement a kind of 'dependency injection'.
    //   To inject a service in a component, attach the desired service placed 
    //   on $root to some property in the 'beforeCreate' component event.
    beforeCreate: function() {

        // Monitor service
        this.$root.$monitorService = 
            this.$root.$monitorService || new MonitorService();
    }
});


Vue.component('alarm-history', {
    props: {
        showAlarmHistory: false
    },
    data: function () {
        return {
            numberOfAlarms: 100,
            alarmsPerPage: 15,
            rows: [],
            alarms: [],
            info: [],
            selectedRow: 0
        }
    },
    beforeCreate() {
        this.$monitorService = this.$root.$monitorService;
    },
    created: function () {
        this.$monitorService.getLastAlarms(this.numberOfAlarms, response => {
            this.alarms = response.list;
            // console.log('ALARMS ', this.alarms);
        })

        this.$on("getAlarmInfo", alarmId => {
            this.showInfoById(alarmId);
        })

        this.$on("updateRows", rows => {
            this.rows = rows;
        })
    },
    template: `
        <div class="alarm-history">
            <div class="content -expand">
                <div class="title">Histórico de Alarmes</div>
                <div class="spacer"></div>
                <div class="table-wrapper">
                    <table class="alarm-table">
                        <thead>
                            <tr class="header-row">
                                <th>Data</th>
                                <th>Hora</th>
                                <th>Duração do Alerta</th>
                                <th>Alerta</th>
                                <th>Valor</th>
                            </tr>
                        </thead>
                        <tbody>
                            <history-row v-for="row in rows" v-bind:date="row.date" v-bind:time="row.time" v-bind:duration="row.duration.slice(2, 7)"
                            v-bind:name="row.message" v-bind:value="row.value ? row.value + ' ' + row.unit : '---'" v-bind:id="row.id" />
                        </tbody>
                        <tfoot></tfoot>
                    </table>
                </div>
                <div class="footer">
                    <div class="programables" v-visible="selectedRow !== 0" v-on:click="hideInfo">
                        <parameter-info v-for="p in info" v-bind:name="p.name" v-bind:value="p.value" v-bind:unit="p.unit"/>
                    </div>
                </div>
            </div>
            
            <div class="btn-group -fEnd">
                <pagination v-bind:items="alarms" v-bind:itemsPerPage="alarmsPerPage"/>
                <a v-on:click="hideAlarmHistory" class="btn -outline-alt">Voltar</a>
            </div>
        </div>
    `,
    methods: {
        showInfoById(alarmId) {
            this.$monitorService.getAlarmInfo(alarmId, response => {
                this.info = response.info;
                this.selectedRow = alarmId;
             })
        },
        hideInfo() {
            this.selectedRow = 0;
        },
        hideAlarmHistory() {
            this.$parent.showAlarmHistory = false;
        }
    }
})
