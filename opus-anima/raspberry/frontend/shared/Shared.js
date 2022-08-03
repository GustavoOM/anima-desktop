Vue.mixin({
    methods: {
        mapAgeGroupToLabel: ageGroup => {
            switch(ageGroup) {
                case 'child': return "Pediátrico";
                case 'adult': return "Adulto";
            }
        },
        mapGenderValueToLabel: genderValue => {
            switch(genderValue) {
                case 'male': return "Masculino";
                case 'female': return "Feminino";
                default: return "Não definido";
            }
        },
        formatNumericalValue: (value, unit) => {
            let displayValue = value > 0 ? value : '-';
            let displayUnit = value > 0 ? unit : '';
            return `${displayValue} ${displayUnit}`;
        },
        getShapeOptions: () => [
            { id: "shape", value: 2, range: [1,4], default: 1, step: 1, unit: '', type: "curve" },
            { id: "shape", value: 1, range: [1,4], default: 1, step: 1, unit: '', type: "square", },
            { id: "shape", value: 3, range: [1,4], default: 1, step: 1, unit: '', type: "descendant" },
            { id: "shape", value: 4, range: [1,4], default: 1, step: 1, unit: '', type: "ascendant" }
        ],
        getImageFromIconLabel: (iconLabel) => {
            return `./images/icon-${iconLabel}.svg`;
        }
        
    },

    beforeCreate: function() {
        String.prototype.truncate = function(n){
          return this.substr(0,n-1)+(this.length>n?'...':'');
        };
        Number.prototype.countDecimals = function () {
            if(Math.floor(this.valueOf()) === this.valueOf()) return 0;
            return this.toString().split(".")[1].length || 0; 
        };
        Number.prototype.setPrecision = function (precision) {
            return parseFloat(this.valueOf()).toFixed(precision); 
        }
        Array.prototype.shuffle = function() {
            let array = this.valueOf();
            var currentIndex = array.length, temporaryValue, randomIndex;
            // While there remain elements to shuffle...
            while (0 !== currentIndex) {
              // Pick a remaining element...
              randomIndex = Math.floor(Math.random() * currentIndex);
              currentIndex -= 1;
              // And swap it with the current element.
              temporaryValue = array[currentIndex];
              array[currentIndex] = array[randomIndex];
              array[randomIndex] = temporaryValue;
            }
            return array;
        }
    }
})
