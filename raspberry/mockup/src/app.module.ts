import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AnimaApiController } from './controllers/anima-api.controller';
import { AnimaSocketController } from './controllers/anima-socket.controller';
import { AppService } from './app.service';

@Module({
  imports: [],
  controllers: [
    AppController, 
    AnimaApiController,
    AnimaSocketController
  ],
  providers: [AppService],
})
export class AppModule {}
