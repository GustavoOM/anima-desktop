import { INestApplication } from "@nestjs/common";
import { DocumentBuilder, SwaggerModule } from "@nestjs/swagger";

export function configureSwagger(app: INestApplication) {

    const options = new DocumentBuilder()
      .setTitle('Opus.Anima.Mockup')
      .setDescription('Opus Ânima API for mockup.')
      .setVersion('1.0')
      .build();
  
    const document = SwaggerModule.createDocument(app, options);
  
    SwaggerModule.setup('api', app, document);

}