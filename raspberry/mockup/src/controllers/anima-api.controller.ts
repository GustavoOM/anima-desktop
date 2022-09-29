import { Param } from '@nestjs/common';
import { Controller, Get, Post, Body } from '@nestjs/common';
import { ApiTags } from '@nestjs/swagger';

@Controller('')
@ApiTags('anima-api')
export class AnimaApiController {
  constructor() {}

  @Get('modes')
  getModes() {
    return { 
        modes:[ 
          { id: 'vcv', label: 'VCV' },
          { id: 'pcv', label: 'PCV' },
          { id: 'psv', label: 'PSV' }
        ]
    }
  }

  @Get('settings')
  getSettings() {
      return { 
          modes:
          [
              {
                  id: 'vcv',
                  mode: "VCV",
                  configuration: [
                    { id: 'FIO2', name: 'FIO2', default: 21, range: [21,100], step: 1, unit: '%' },
                    { id: 'shape', name: 'Shape', default: 1, range: [1,4], step: 1, unit: '' },
                    { id: 'tIns', name: 'T.Ins', default: 1000, range: [200,9000], step: 50, unit: 'ms' },
                    { id: 'Freq', name: 'Freq', default: 20, range: [1,80], step: 1, unit: 'BPM' },
                    { id: 'vTidal', name: 'V Tidal', default: 500, range: [50, 2000], step: 10, unit: 'ml' },
                    { id: 'PEEP', name: 'PEEP', default: 10, range: [0,35], step: 1, unit: 'cmH2O' },
                    { id: 'sensInsP', name: 'SensInsP', default: 0, range: [-15,0], step: 1, unit: 'cmH2O' },
                    { id: 'sensInpF', name: 'SensInsF', default: 0, range: [0, 10], step: 1, unit: 'L/min' },
                    { id: 'tPausaIns', name: 'Pausa Ins', default: 0, range: [0, 4000], step: 40, unit: 'ms' },
                    { id: 'pInsLimit', name: 'P Limite', default: 45, range: [40, 80], step: 1, unit: 'cmH2O' },
                  ]
              },
              {
                  id: 'pcv',
                  mode: "PCV",
                  configuration: [
                    { id: 'FIO2', name: 'FIO2', default: 21, range: [21,100], step: 1, unit: '%' },
                    { id: 'riseTime', name: 'R.Time', default: 0, range: [0, 2000], step: 100, unit: 'ms' },
                    { id: 'tIns', name: 'T.Ins', default: 1000, range: [200,9000], step: 50, unit: 'ms' },
                    { id: 'Freq', name: 'Freq', default: 20, range: [1,80], step: 1, unit: 'BPM' },
                    { id: 'pCon', name: 'P Con', default: 20, range: [5,70], step: 1, unit: 'cmH2O' },
                    { id: 'PEEP', name: 'PEEP', default: 10, range: [0,35], step: 1, unit: 'cmH2O' },
                    { id: 'sensInsP', name: 'SensInsP', default: 0, range: [-15,0], step:1, unit: 'cmH2O' },
                    { id: 'sensInpF', name: 'SensInsF', default: 0, range: [0, 10], step: 1, unit: 'L/min' },
                  ]
              },
              {
                  id: 'psv',
                  mode: "PSV",
                  configuration: [
                    { id: 'FIO2', name: 'FIO2', default: 21, range: [21,100], step: 1, unit: '%' },
                    { id: 'riseTime', name: 'R.Time', default: 0, range: [0, 2000], step: 20, unit: 'ms' },
                    { id: 'tInsMax', name: 'T.Ins MAX', default: 10000, range: [100, 25000], step: 100, unit: 'ms' },
                    { id: 'pSupport', name: 'P Suporte', default: 20, range: [5, 70], step: 1, unit: 'cmH20' },
                    { id: 'PEEP', name: 'PEEP', default: 10, range: [0,35], step: 1, unit: 'cmH2O' },
                    { id: 'sensInsP', name: 'SensInsP', default: 0, range: [-15,0], step:1, unit: 'cmH2O' },
                    { id: 'sensInpF', name: 'SensInsF', default: 0, range: [0, 10], step: 1, unit: 'L/min' },
                    { id: 'sensExpF', name: 'SensExpF', default: 15, range: [0, 30], step: 1, unit: 'L/min' },
                    { id: 'bkMode', name: 'Modo BK', default: 1, range: [1,2], step: 1, unit: '' },
                    { id: 'bkTIns', name: 'T.Insp BK', default: 1000, range: [200, 9000], step: 100, unit: 'ms' },
                    { id: 'bkFreq', name: 'Frequência BK', default: 20, range: [1, 80], step: 1, unit: 'BPM' },
                    { id: 'bkVTidal', name: 'V.Tidal BK', default: 500, range: [50, 2000], step: 10, unit: 'ml' },
                    { id: 'bkPcon', name: 'P Controlada BK', default: 20, range: [5, 70], step: 1, unit: 'cmH2O' },
                    { id: 'notBreathing', name: 'Apneia', default: 20, range: [2, 60], step: 1, unit: 's' },
                  ],
                  
              }
          ],
          alarms: {
            min: [
              { id: 'fio2min', name: 'FIO2 [min]', default: 21, range: [10,50], step: 1, unit: '%' },
              { id: 'vMinMin', name: 'V Minuto [min]', default: 1, range: [0.1,1.0], step: 0.01, unit: 'L' },
              { id: 'pInsMin', name: 'P Pico [min]', default: 5, range: [1.0,60.0], step: 0.50, unit: 'cm H2O' },
              { id: 'vTidalInsMin', name: 'V Tidal [min]', default: 100, range: [10,400], step: 5, unit: 'ml' },
            ],
            max: [
              { id: 'fio2max', name: 'FIO2 [max]', default: 90, range: [70,100], step: 1, unit: '%' },
              { id: 'vMinMax', name: 'V Minuto [max]', default: 30, range: [1.0,60.0], step: 0.6, unit: 'L' },
              { id: 'pInsMax', name: 'P Pico [max]', default: 40, range: [5.0,100.0], step: 1.00, unit: 'cm H2O' },
              { id: 'vTidalInsMax', name: 'V Tidal [max]', default: 1000, range: [400,2500], step: 20, unit: 'ml' },
              { id: 'mFreq', name: 'Freq [max]', default: 80, range: [10,80], step: 1, unit: 'BPM' },
              { id: 'maxPEEP', name: 'PEEP [max]', default: 10, range: [1,35], step: 1, unit: 'cm H2O' },
            ],
            set: [
              // { id: 'notBreathing', name: 'Apneia', default: 20, range: [2,60], step: 1, unit: 's' },
            ]
          },
          output: [
              { id: 'plateau50ms', name: "P Plateau", range: [0,60], unit: "cm H2O" },
              { id: 'mFIO2', name: "FIO2", range: [ 10, 100 ], unit: "%" },
              { id: 'vMinIns', name: "V Minuto", range: [0.1,60], unit: "L" },
              { id: 'peep50ms', name: "PEEP", range: [0,35], unit: "cm H20" },
              { id: 'pPeek', name: "P Pico", range: [0,100], unit: "cm H20" },
              { id: 'pAverage', name: "P Media", range: [0,60], unit: "cm H20" }, 
              { id: 'mTIns', name: "T Ins", range: [0,2500], unit: "ms" },
              { id: 'mFreq', name: "Freq", range: [0,180], unit: "BPM" },
              { id: 'vTidalIns', name: "V Tidal", range: [10,2500], unit: "ml" },
              { id: 'cStat', name: "C Est", range: [0,100], unit: "ml/cmH2O" },
            ],
            graphics: [
              { id: 'flow', name: "Fluxo", xname: 'Fluxo', yname: 'Tempo', xunit:"s", yunit:"L/min", yrange: [-40,40], xrange: [0,20] },
              { id: 'pressure', name: "Pressão", xname: 'Pressão', yname: 'Tempo', xunit:"s", yunit:"cmH20", yrange: [0,35], xrange: [0,20] },
              { id: 'volume', name: "Volume", xname: 'Volume', yname: 'Tempo', xunit:"s", yunit:"ml", yrange: [-0.2,0.8], xrange: [0,20] }
            ]
          
      };
  }

  @Get('log/list-:n')
  getLogList(@Param('n') n) {
    return { 
        list:[ 
          { id: 1, date: 'vcv', time: 'VCV', duration: ['1', '2'], message: 'Baixo nível de oxigênio.', value: '1', unit: 's' },
          { id: 2, date: 'vcv', time: 'VCV', duration: ['1', '2'], message: 'Baixo nível de oxigênio.', value: '15', unit: 's' },
          { id: 3, date: 'vcv', time: 'VCV', duration: ['1', '2'], message: 'Baixo nível de oxigênio.', value: '150', unit: 's' }
        ]
    }
  }

  @Post('configure')
  setConfigure(@Body() obj: {}) {
      return obj;
  }

  @Post('boundaries')
  setAlarmTriggers(@Body() obj: {}) {
      return obj;
  }

  @Post('patient')
  addPatiente(@Body() obj: {}) {
      return obj;
  }

}
