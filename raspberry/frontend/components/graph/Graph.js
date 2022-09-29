const ticksNumber = 14;
// const demandedOrder = [1, 2, 0, 3]

Vue.component('graph-frame', {
    props: {
        title: String,
        xunit: String,
        yunit: String,
        id: String,
        height: Number,
        width: Number,
        margin: {},
        yaxis: [],
        xaxis: [],
        color: '',
        axisPosition: ''
    },
    data: function () {
        return {
        }
    },
    template: `<div :id="id"  class="graphStyle">
                    <p class="title">{{title}} 
                        <span v-show="id === 'graph3'" class="unit">{{yunit}} / {{xunit}}</span>
                        <span v-show="id !== 'graph3'" class="unit">{{yunit}}</span>
                    </p>
                </div>`,
    created: function () {
        this.$parent.$on(`draw${this.id}`, (dataset, overwrite) => {
            if (dataset != undefined) {
                if(this.id === 'graph3')
                    this.drawLoopLine(dataset, overwrite);
                else
                    this.drawLine(dataset, overwrite);
            }
        });
    },
    mounted: function() {
        this.createGraphicScope();
    },
    methods: {
        createGraphicScope() {
            if (this.id !== 'graph3') {
                var svg = d3.select(`#${this.id}`)
                    .append("svg")
                    .attr("width", this.width + this.margin.left + this.margin.right)
                    .attr("height", this.height + this.margin.top + this.margin.bottom)
                    .append("g")
                    .attr("transform",
                        "translate(" + this.margin.left + "," + this.margin.top + ")");

                var xScale = d3.scaleLinear()
                    .domain([this.xaxis[0], this.xaxis[1]])
                    .range([0, this.width]);

                // Volume graph does not show negative values
                if (this.id === 'graph2')
                    this.yaxis[0] = 0;

                var yScale = d3.scaleLinear()
                    .domain([this.yaxis[0], this.yaxis[1]])
                    .range([this.height, 0]);

                var xAxis = d3.axisBottom()
                    .ticks(4)
                    .scale(xScale);

                var yAxis = d3.axisLeft()
                    .ticks(ticksNumber)
                    .scale(yScale)
                    .tickSize(-this.width);

                // Defines xAxis transform position based on axisPostion
                var xAxisHeightPos =
                    this.axisPosition == 'bottom' ? this.height :     // bottom
                        this.axisPosition == 'center' ? this.height / 2 :   // center
                            0            // top


                svg.append("g")
                    .attr("class", "y-axis")
                    .call(yAxis);

                svg.append("g")
                    .attr("class", "x-axis")
                    .attr("transform", "translate(0," + xAxisHeightPos + ")")
                    .call(xAxis);

                d3.selectAll(".x-axis:first-child .tick")
                    .each(function (tickValue, index) {
                        if ((tickValue % 5) != 0) {
                            this.querySelector('text').innerHTML = "";
                        }
                    });

                // When axisPosition is 'center', remove tick 0 from x axis 
                if (this.axisPosition == 'center')
                    d3.selectAll(".x-axis:first-child .tick")
                        .each(function (tickValue) { if (tickValue == 0) this.remove(); });

            } else {

                var svg = d3.select(`#${this.id}`)
                    .append("svg")
                    .attr("width", this.width + this.margin.left + this.margin.right)
                    .attr("height", this.height + this.margin.top + this.margin.bottom)
                    .append("g")
                    .attr("transform",
                        "translate(" + this.margin.left + "," + this.margin.top + ")");

                var xScale = d3.scaleLinear()
                    .domain([this.xaxis[0], this.xaxis[1]])
                    .range([0, this.width]);

                var yScale = d3.scaleLinear()
                    .domain([this.yaxis[0], this.yaxis[1]])
                    .range([this.height, 0]);

                var xAxis = d3.axisBottom()
                    .ticks(ticksNumber * 0.5)
                    .scale(xScale)
                    .tickSize(-5);

                var yAxis = d3.axisLeft()
                    .ticks(ticksNumber)
                    .scale(yScale)
                    .tickSize(-5);

                // Defines xAxis transform position based on axisPostion
                var xAxisHeightPos =
                    this.axisPosition == 'bottom' ? this.height :     // bottom
                        this.axisPosition == 'center' ? this.height / 2 :   // center
                            0            // top


                svg.append("g")
                    .attr("class", "y-axis-loop")
                    .call(yAxis);

                svg.append("g")
                    .attr("class", "x-axis-loop")
                    .attr("transform", "translate(0," + xAxisHeightPos + ")")
                    .call(xAxis);

                d3.selectAll(".x-axis-loop:first-child .tick")
                    .each(function (tickValue, index) {
                        if ((tickValue % 5) != 0) {
                            this.querySelector('text').innerHTML = "";
                        }
                    });

                // When axisPosition is 'center', remove tick 0 from x axis 
                if (this.axisPosition == 'center')
                    d3.selectAll(".x-axis-loop:first-child .tick")
                        .each(function (tickValue) { if (tickValue == 0) this.remove(); });
            }
        },
        drawLine(dataset, overwrite = true) {
            if (overwrite)
                d3.selectAll(`.line${this.id}`).remove();

            var xScale = d3.scaleLinear()
                .domain([this.xaxis[0], this.xaxis[1]])
                .range([0, this.width]);

            var yScale = d3.scaleLinear()
                .domain([this.yaxis[0], this.yaxis[1]])
                .range([this.height, 0]);

            d3.select(`#${this.id} svg>g`).append("path")
                .datum(dataset)
                .attr("fill", "none")
                .attr("stroke", this.color)
                .attr("stroke-width", 1.5)
                .attr("class", `line${this.id}`)
                .attr("d", d3.line()
                    .curve(d3.curveLinear)
                    .x(function (d) { return xScale(d.x) })
                    .y(function (d) { return yScale(d.y) })
                );
        },
        drawLoopLine(dataset, overwrite = true) {
            if (overwrite)
                d3.selectAll(`.line${this.id}`).remove();

            var xScale = d3.scaleLinear()
                .domain([this.xaxis[0], this.xaxis[1]])
                .range([0, this.width]);

            var yScale = d3.scaleLinear()
                .domain([this.yaxis[0], this.yaxis[1]])
                .range([this.height, 0]);

            // d3.select(`#${this.id} svg>g`).append("linearGradient")
            //     .attr("id", "line-gradient")
            //     .selectAll("stop")
            //     .data([
            //         {offset: "0%", color: "orange"},
            //         {offset: "100%", color: this.color}
            //     ])
            //     .enter().append("stop")
            //         .attr("offset", function(d) { return d.offset; })
            //         .attr("stop-color", function(d) { return d.color; });

            d3.select(`#${this.id} svg>g`).append("path")
                .datum(dataset)
                .attr("fill", "none")
                .attr("stroke", this.color)
                .attr("stroke-width", 1.5)
                .attr("class", `line${this.id}`)
                .attr("d", d3.line()
                    .curve(d3.curveLinear)
                    .x(function (d) { return xScale(d.x) })
                    .y(function (d) { return yScale(d.y) })
                );
        },
        // Updates axis ranges with current values defined on parent
        updateLimits() {
            if (this.id !== 'graph3') {
                var yScale = d3.scaleLinear()
                    .domain([this.yaxis[0], this.yaxis[1]])
                    .range([this.height, 0]);
    
                var yAxis = d3.axisLeft()
                    .ticks(ticksNumber)
                    .scale(yScale)
                    .tickSize(-this.width);
    
                d3.select(`#${this.id} svg>g .y-axis`)
                    .call(yAxis);
            } else {
                var yScale = d3.scaleLinear()
                    .domain([this.yaxis[0], this.yaxis[1]])
                    .range([this.height, 0]);
    
                var yAxis = d3.axisLeft()
                    .ticks(ticksNumber)
                    .scale(yScale)
                    .tickSize(-5);
    
                d3.select(`#${this.id} svg>g .y-axis-loop`)
                    .call(yAxis);

                var xScale = d3.scaleLinear()
                    .domain([this.xaxis[0], this.xaxis[1]])
                    .range([0, this.width]);

                var xAxis = d3.axisBottom()
                    .ticks(ticksNumber * 0.5)
                    .scale(xScale)
                    .tickSize(-5);

                d3.select(`#${this.id} svg>g .x-axis-loop`)
                    .call(xAxis);
            }
        }
    }
})

// function drawGraph(dataset, overwrite = true) {
//     if (overwrite)
//         d3.selectAll(`.line-${this.id}`).remove();

//     svg.append("path")
//         .datum(dataset)
//         .attr("fill", "none")
//         .attr("stroke", this.color)
//         .attr("stroke-width", 1.5)
//         .attr("class", `line-${this.id}`)
//         .attr("d", d3.line()
//             .curve(d3.curveLinear)
//             .x(function (d) { return xScale(d.x) })
//             .y(function (d) { return yScale(d.y) })
//         );

    // // Another Color
    // svg.append("path")
    //     .datum([{ key: 0, value: 2 },
    //         { key: 0, value: 1.2 },
    //         { key: 1, value: 1.5 },
    //         { key: 2, value: 2.3 },
    //         { key: 3, value: 1.5 },
    //         { key: 4, value: 4.1 }])
    //     .attr("fill", "none")
    //     .attr("stroke", "blue")
    //     .attr("stroke-width", 1.5)
    //     .attr("d", d3.line()
    //         .curve(d3.curveBasis)
    //         .x(function (d) { return xScale(d.key) })
    //         .y(function (d) { return yScale(d.value) })
    //     );
// }
