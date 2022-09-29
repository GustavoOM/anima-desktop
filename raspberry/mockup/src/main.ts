import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { configureSwagger } from './app.configuration';

declare const module: any;

async function bootstrap() {

  const app = await NestFactory.create(AppModule);
  
  app.enableCors();
  configureSwagger(app)

  if (module.hot) {
    module.hot.accept();
    module.hot.dispose(() => app.close());
  }

  await app.listen(5000);
}

bootstrap();
