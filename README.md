# Projeto-de-irriga-o-inteligente
O projeto denominado Irrigação Inteligente com uso de Plataformas de Baixo Custo, desenvolvido na Instituição Centro de Ensino Tecnológico CENTEC FATEC CARIRI, pelos alunos do curso de Tecnologia em Menutenção Industrial, têm como principal objetivo apresentar aos pequenos produtores da região rural do Cariri o acesso a tecnologias que podem fornecer uma melhor produtividade e conforto para as plantações e áreas de cultivo. Os principais componentes contidos no protótipo são: uma bomba de água de pequeno ou médio porte, uma placa microcontrolada ESP32 ou ESP8266, na qual se destaca por permitir a conexão com WiFi, um relé 5V para conexão da bomba com a placa, um sensor HC-SR04, sensor de umidade de solo, junpers (fios) e uma protoboard para montagem do circuito. Pode-se realizar a confecção de uma placa única para tornar o projeto mais compacto e profissional. O uso da internet é imprecindível para poder permitir a conexão da placa microcontrolada, porém torna-se um desafio por o acesso econômico, mas tendo em vista que nos dias atuais o acesso a serviçõs de internet estão cada vez mais facilitados, o projeto se torna viável para aplicaçã. A lógica do código para o funcionamento do projeto se baseia principalmente em dois fatores principais. Um se refere a umidade do solo e outro ao nível de água contido no reservatório. A bomba irá ser acionada através do monitoramento da umidade do solo, ou seja, quando o sensor de umidade indentificar que o solo estela seco, mas que o nível de água também esteja alto no reservatório. Isso foi pensado com o intuito de conservar o estado da bomba de água, tendo em vista que se não estiver água suficiente no reservatório, a mesma venha a queimar. Tanto os valores de umidade de solo quanto o nível do reservatório podem ser visualizados em uma interface no aplicativo Blynk, onde o mesmo permite a conexão e troca de dados com placas ESP32, ESP8266, etc.
    
    
