Vue.component('pagination', {
    props: {
        items: [],
        itemsPerPage: '',
    },
    watch: {
        items: function() {
            this.totalPages = Math.ceil(this.items.length / this.itemsPerPage);
            this.updateItems();
        }
    },
    data: function() {
        return {
            currentPage: 1,
            totalPages: '',
            pages: [],
            itemsDisplayed: ''
        }
    },
    created: function () {
        this.totalPages = Math.ceil(this.items.length / this.itemsPerPage);
        this.updateItems();
    },
    template: `
        <div class="btn-group pagination">  
            <button v-on:click="previousPage" class="btn -outline-alt">Anterior</button>
                <button v-for="page in totalPages" 
                    v-on:click="goToPage(page)" 
                    class="btn -outline-alt pageNumber" v-bind:class="{ 'selected': currentPage === page }">
                    {{ page }}
                </button>
            <button v-on:click="nextPage" class="btn -outline-alt">Próxima</button>
        </div>
    `,
    methods: {
        nextPage() {
            if (this.currentPage == this.totalPages) {
                return;
            }
            this.currentPage++;
            this.updateItems();
        },
        previousPage() {
            if (this.currentPage == 1) {
                return;
            }
            this.currentPage--;
            this.updateItems();
        },
        goToPage(page) {
            this.currentPage = page;
            this.updateItems();
        },
        updateItems() {
            firstItem = (this.currentPage - 1) * this.itemsPerPage;
            lastItem = (firstItem + this.itemsPerPage);
            lastItem = lastItem <= this.items.length ? lastItem : this.items.length;

            this.itemsDisplayed = this.items.slice(firstItem, lastItem);

            this.$parent.$emit('updateRows', this.itemsDisplayed);
        }
    }
})