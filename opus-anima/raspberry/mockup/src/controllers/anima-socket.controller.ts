import { Controller, Get, Post, Body } from '@nestjs/common';
import { ApiTags } from '@nestjs/swagger';

@Controller('')
@ApiTags('anima-socket')
export class AnimaSocketController {
  constructor() {}

  @Get('graphics/state-1')
  getGraphics_State1() {
    return { 

        graphics: [

            {
                name: "Fluxo", 
                timestamp: 12345678,
                yunit: "L/min",
                xunit: 's',
                value:[
                    { x:0,  y:2.3 },
                    { x:1,  y:13.3 },
                    { x:2,  y:-10.5 },
                    { x:3,  y:-10.0 },
                    { x:4,  y:13.3 },
                    { x:5,  y:24.0 },
                    { x:6,  y:2.3 },
                    { x:7,  y:-11.3 },
                    { x:8,  y:-11.8 },
                    { x:9,  y:0.7 },
                    { x:10,  y:30.0 },
                    { x:11,  y:-10.3 },
                    { x:12,  y:-10.7 },
                    { x:13,  y:-10.3 },
                    { x:14,  y:-11.3 },
                    { x:15,  y:-10.3 },
                    { x:16,  y:-15.3 },
                    { x:17,  y:-10.3 },
                    { x:18,  y:15.3 },
                    { x:19,  y:-10.3 },
                    { x:20,  y:2.3 }
                ]
            },
            
            {
                name: "Pressão",
                yunit: "cmH2O",
                xunit: 's', 
                timestamp: 12345678, 
                value:[
                    { x:0,  y:2.3 },
                    { x:1,  y:3.3 },
                    { x:2,  y:10.5 },
                    { x:3,  y:10.0 },
                    { x:4,  y:33.3 },
                    { x:5,  y:24.0 },
                    { x:6,  y:22.3 },
                    { x:7,  y:11.3 },
                    { x:8,  y:11.8 },
                    { x:9,  y:20.7 },
                    { x:10,  y:10.0 },
                    { x:11,  y:10.3 },
                    { x:12,  y:10.7 },
                    { x:13,  y:10.3 },
                    { x:14,  y:11.3 },
                    { x:15,  y:10.3 },
                    { x:16,  y:15.3 },
                    { x:17,  y:10.3 },
                    { x:18,  y:25.3 },
                    { x:19,  y:10.3 },
                    { x:20,  y:22.3 }
                ]
            },
            {
                name: "Volume",
                yunit: "L",
                xunit: 's',
                timestamp: 12345678, 
                value:[
                    { x:0,  y:0.22 },
                    { x:1,  y:0.33 },
                    { x:2,  y:0.10 },
                    { x:3,  y:0.10 },
                    { x:4,  y:0.33 },
                    { x:5,  y:0.44 },
                    { x:6,  y:0.22 },
                    { x:7,  y:0.11 },
                    { x:8,  y:0.11 },
                    { x:9,  y:0.20 },
                    { x:10,  y:0.23 },
                    { x:11,  y:0.18 },
                    { x:12,  y:0.17 },
                    { x:13,  y:0.10 },
                    { x:14,  y:0.11 },
                    { x:15,  y:0.10 },
                    { x:16,  y:0.15 },
                    { x:17,  y:0.10 },
                    { x:18,  y:0.35 },
                    { x:19,  y:0.10 },
                    { x:20,  y:0.22 }
                ]
            },
        ]
    }
  }

  @Get('graphics/state-2')
  getGraphics_State2() {
    return { 

        graphics: [

            {
                name: "Fluxo",
                yunit: "cmH20",
                xunit: 's', 
                timestamp: 12345678, 
                value:[
                    { x:0,  y:2.3 },
                    { x:1,  y:13.3 },
                    { x:2,  y:-10.5 },
                    { x:3,  y:-10.0 },
                    { x:4,  y:13.3 },
                    { x:5,  y:24.0 },
                    { x:6,  y:2.3 },
                    { x:7,  y:-11.3 },
                    { x:8,  y:-11.8 },
                    { x:9,  y:0.7 },
                    { x:10,  y:30.0 },
                    { x:11,  y:10.3 },
                    { x:12,  y:4.7 },
                    { x:13,  y:5.3 },
                    { x:14,  y:-1.3 },
                    { x:15,  y:10.3 },
                    { x:16,  y:-5.3 },
                    { x:17,  y:10.3 },
                    { x:18,  y:15.3 },
                    { x:19,  y:-10.3 },
                    { x:20,  y:2.3 }
                ]
            },
            
            {
                name: "Pressão",
                yunit: "ml/min",
                xunit: 's', 
                timestamp: 12345678, 
                value:[
                    { x:0,  y:2.3 },
                    { x:1,  y:3.3 },
                    { x:2,  y:10.5 },
                    { x:3,  y:10.0 },
                    { x:4,  y:33.3 },
                    { x:5,  y:24.0 },
                    { x:6,  y:22.3 },
                    { x:7,  y:11.3 },
                    { x:8,  y:11.8 },
                    { x:9,  y:20.7 },
                    { x:10,  y:23.0 },
                    { x:11,  y:24.3 },
                    { x:12,  y:24.7 },
                    { x:13,  y:24.3 },
                    { x:14,  y:11.3 },
                    { x:15,  y:10.3 },
                    { x:16,  y:15.3 },
                    { x:17,  y:10.3 },
                    { x:18,  y:25.3 },
                    { x:19,  y:10.3 },
                    { x:20,  y:22.3 }
                ]
            },
            {
                name: "Volume",
                yunit: "ml",
                xunit: 's', 
                timestamp: 12345678, 
                value:[
                    { x:0,  y:0.22 },
                    { x:1,  y:0.33 },
                    { x:2,  y:0.10 },
                    { x:3,  y:0.10 },
                    { x:4,  y:0.33 },
                    { x:5,  y:0.44 },
                    { x:6,  y:0.22 },
                    { x:7,  y:0.11 },
                    { x:8,  y:0.11 },
                    { x:9,  y:0.20 },
                    { x:10,  y:0.23 },
                    { x:11,  y:0.28 },
                    { x:12,  y:0.27 },
                    { x:13,  y:0.40 },
                    { x:14,  y:0.41 },
                    { x:15,  y:0.40 },
                    { x:16,  y:0.25 },
                    { x:17,  y:0.20 },
                    { x:18,  y:0.35 },
                    { x:19,  y:0.10 },
                    { x:20,  y:0.22 }
                ]
            },
        ]
    }
  }

  @Get('graphics/history')
  getGraphics_History() {
    return { 

        graphics: [
            
            {
                name: "Fluxo", 
                timestamp: 12345678, 
                value:[
                    { x:-12, y:22.3 },
                    { x:-11, y:33.3 },
                    { x:-10, y:10.5 },
                    { x:-9, y:10.0 },
                    { x:-8, y:33.3 },
                    { x:-7, y:44.0 },
                    { x:-6, y:22.3 },
                    { x:-5, y:11.3 },
                    { x:-4, y:11.8 },
                    { x:-3, y:20.7 },
                    { x:-2, y:50.0 },
                    { x:-1, y:10.3 },
                    { x:0, y:10.7 },
                    { x:1, y:33.3 },
                    { x:2, y:10.5 },
                    { x:3, y:10.0 },
                    { x:4, y:33.3 },
                    { x:5, y:44.0 },
                    { x:6, y:22.3 },
                    { x:7, y:11.3 },
                    { x:8, y:11.8 },
                    { x:9, y:20.7 },
                    { x:10, y:50.0 },
                    { x:11, y:10.3 },
                    { x:12, y:10.7 }
                ]
            },
            
            {
                name: "Pressão",
                unit: "l/min", 
                timestamp: 12345678, 
                value:[
                    { x:-12, y:22.3 },
                    { x:-11, y:33.3 },
                    { x:-10, y:10.5 },
                    { x:-9, y:10.0 },
                    { x:-8, y:33.3 },
                    { x:-7, y:44.0 },
                    { x:-6, y:22.3 },
                    { x:-5, y:11.3 },
                    { x:-4, y:11.8 },
                    { x:-3, y:20.7 },
                    { x:-2, y:50.0 },
                    { x:-1, y:10.3 },
                    { x:0, y:10.7 },
                    { x:1, y:33.3 },
                    { x:2, y:10.5 },
                    { x:3, y:10.0 },
                    { x:4, y:33.3 },
                    { x:5, y:44.0 },
                    { x:6, y:22.3 },
                    { x:7, y:11.3 },
                    { x:8, y:11.8 },
                    { x:9, y:20.7 },
                    { x:10, y:50.0 },
                    { x:11, y:10.3 },
                    { x:12, y:10.7 }
                ]
            },
            {
                name: "Volume",
                unit: "ml", 
                timestamp: 12345678, 
                value:[
                    { x:-12, y:22.3 },
                    { x:-11, y:33.3 },
                    { x:-10, y:10.5 },
                    { x:-9, y:10.0 },
                    { x:-8, y:33.3 },
                    { x:-7, y:44.0 },
                    { x:-6, y:22.3 },
                    { x:-5, y:11.3 },
                    { x:-4, y:11.8 },
                    { x:-3, y:20.7 },
                    { x:-2, y:50.0 },
                    { x:-1, y:10.3 },
                    { x:0, y:10.7 },
                    { x:1, y:33.3 },
                    { x:2, y:10.5 },
                    { x:3, y:10.0 },
                    { x:4, y:33.3 },
                    { x:5, y:44.0 },
                    { x:6, y:22.3 },
                    { x:7, y:11.3 },
                    { x:8, y:11.8 },
                    { x:9, y:20.7 },
                    { x:10, y:50.0 },
                    { x:11, y:10.3 }
                ]
            },
        ]
    }
  }

  @Get('sensors/state-1')
  getSensors_State1() {
    return { 

        output: [
            { id: 'mFIO2', name: "FIO2", range: [ 10, 100 ], value: "57" },
            { id: 'vMinIns', name: "V Minuto", range: [0.1,60], value: "62" },
            { id: 'peep50ms', name: "PEEP", range: [0,35], value: "11000" },
            { id: 'pPeek', name: "P Pico", range: [0,100], value: "45" },
            { id: 'pAverage', name: "P Media", range: [0,60], value: "45" }, 
            { id: 'mTIns', name: "T Ins", range: [0,2500], value: "500" },
            { id: 'mFreq', name: "Freq", range: [0,180], value: "75" },
            { id: 'vTidalIns', name: "V Tidal", range: [10,2500], value: "49" },
            { id: 'plateau50ms', name: "P Plateau", range: [0,60], value: "30" },
            { id: 'cStat', name: "C Est", range: [0,100], value: "50" },

        ]
    }
  }

  @Get('sensors/state-2')
  getSensors_State2() {
    return { 

        output: [
            { id: 'mFIO2', name: "FIO2", range: [ 10, 100 ], value: "57" },
            { id: 'vMinIns', name: "V Minuto", range: [0.1,60], value: "62" },
            { id: 'peep50ms', name: "PEEP", range: [0,35], value: "11000" },
            { id: 'pPeek', name: "P Pico", range: [0,100], value: "45" },
            { id: 'pAverage', name: "P Media", range: [0,60], value: "45" }, 
            { id: 'mTIns', name: "T Ins", range: [0,2500], value: "500" },
            { id: 'mFreq', name: "Freq", range: [0,180], value: "75" },
            { id: 'vTidalIns', name: "V Tidal", range: [10,2500], value: "49" },
            { id: 'plateau50ms', name: "P Plateau", range: [0,60], value: "30" },
            { id: 'cStat', name: "C Est", range: [0,100], value: "50" },
        ]
    }
  }

}