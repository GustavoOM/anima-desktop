
Vue.component('confirm-window', {
    props: {
        message: '',
        confirmationCallback: () => {},
    },
    template: `<div class="alert confirm-alert -high">
                    <div class="description">
                        <span>{{ message }}</span>
                    </div>
                    
                    <div class="buttons">
                        <button class="btn" v-on:click="onAccept">Sim</button>
                        <button class="btn" v-on:click="onCancel">Não</button>
                    </div>
                </div>`,
    methods: {
        onAccept() {
            this.confirmationCallback();
            this.$emit('closeWindow');
        },
        onCancel() {
            this.$emit('closeWindow');
        }
    }
})

