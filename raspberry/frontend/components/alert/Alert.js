
Vue.component('alert-pane', {
    props: {
        messages : []
    },
    template: `<div class="alert" v-if="messages.length" v-bind:class="{ '-no-alerts': !messages.length, '-high': messages.length  }">
                    <div class="description">
                        <img v-if="messages.length" src="./images/icon-alert.svg" alt="alerta prioritário">
                        <span v-if="messages[0]">{{ messages[0].message }}</span>
                    </div>
                    <!-- <div class="actions">
                        <div class="icon">
                            <!-- NEED TO CHANGE ON CLICK TO ICON-NOTIFICATION-OFF-WHITE -->
                            <!-- <img src="./images/icon-notification-white.svg" alt="notification-alert"> -->
                        </div>
                        <div class="icon">
                            <img src="./images/icon-close-circle-white.svg" alt="clear notification">
                        </div>
                        <button class="btn -dropdown">
                            <img src="./images/icon-menu-down-white.svg" alt="dropdown">
                        </button>
                    </div> -->
                </div>`
})

