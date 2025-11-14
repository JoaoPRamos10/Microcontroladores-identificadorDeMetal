# Microcontroladores - Identificador De Metal

-----

# 🤖 Projeto: Lixeira Seletiva Automatizada (Arduino)

Um protótipo de lixeira inteligente usando Arduino e sensores para identificar e separar automaticamente resíduos em diferentes categorias.

O objetivo principal deste projeto é criar um sistema funcional e realista que utiliza um sensor indutivo para separar com precisão resíduos em duas categorias centrais: **Metal** e **Não-Metal**.

## 💡 O Desafio: A Limitação dos Sensores

O plano original de separar 4 categorias (metal, papel, vidro, plástico) enfrenta um desafio técnico significativo:

  * **Detecção de Metal:** É 100% viável. O sensor indutivo (**CHE12-4N**) detecta metal de forma confiável.
  * **Detecção de Outros Materiais:** Este é o problema. O sensor de cor (**TCS3200**) não consegue diferenciar o *material*, apenas a *cor*.
      * Um papelão (marrom) é idêntico a um vidro de cerveja (marrom).
      * Um pote de iogurte (branco) é idêntico a um papel sulfite (branco).
      * Um vidro transparente é idêntico a uma garrafa PET transparente.

Por essa razão, este projeto foca em uma solução robusta de 2 categorias (Metal vs. Não-Metal), com o sensor de cor sendo usado de forma experimental em uma segunda fase.

## 🧱 Hardware Utilizado

  * **Cérebro:** Arduino Uno
  * **Detecção de Presença:** Sensor Ultrassônico (HC-SR04)
  * **Detecção de Metal:** Sensor Indutivo (CHE12-4N)
  * **Mecanismo de Separação:** Motor de Passo + Driver (ex: ULN2003)
  * **Estrutura:** Caixa de papelão alta, papelão adicional (para o túnel)
  * **(Experimental)** Detecção de Cor: Sensor de Cor (TCS3200)

## 🏗️ Montagem Física: O Túnel de Inspeção

Para garantir que os sensores façam leituras precisas, o lixo deve ser forçado a passar por um caminho estreito. Usaremos a altura da caixa para criar um "túnel de inspeção" interno.

1.  **Abertura Superior:** Um buraco no topo da caixa para inserir o lixo.
2.  **Túnel de Queda:** Logo abaixo da abertura, construa um "cano" (quadrado ou redondo) de papelão com 8-10 cm de diâmetro. O lixo deve cair por dentro dele.
3.  **Sensor de Presença (HC-SR04):**
      * **Posição:** No **topo** do túnel de queda.
      * **Função:** Fica "olhando" através do túnel. Quando um objeto cai e "corta" o feixe, ele avisa ao Arduino: "Objeto a caminho\!".
4.  **Estação de Identificação (Sensores Principais):**
      * **Posição:** Mais abaixo no túnel (10-15 cm abaixo do HC-SR04).
      * **Sensor de Metal (CHE12-4N):** Montado na lateral do túnel, com a ponta o mais próximo possível do caminho do objeto.
      * **Sensor de Cor (TCS3200):** Montado no lado oposto, apontando para o objeto. (Nota: Esta área deve ser escura para que os LEDs do sensor forneçam a única luz, garantindo uma leitura de cor consistente).
5.  **Mecanismo de Separação (Motor de Passo):**
      * **Posição:** Na **base** da caixa, no fim do túnel.
      * **Função:** O motor gira uma pequena "rampa" ou "alavanca".
          * **Posição A (Padrão):** Rampa direciona o lixo para o compartimento "Não-Metal".
          * **Posição B:** Rampa direciona o lixo para o compartimento "Metal".

## ⚙️ Lógica de Funcionamento

O código do Arduino deve seguir esta sequência:

> 1.  **Esperar:** O loop principal fica monitorando o sensor HC-SR04.
> 2.  **Detectar:** O HC-SR04 detecta um objeto (a distância medida muda bruscamente).
> 3.  **Aguardar (Delay):** O programa espera um tempo fixo (ex: `delay(100)`) para o objeto cair do sensor HC-SR04 até a "Estação de Identificação". *Este tempo precisará ser calibrado experimentalmente.*
> 4.  **Ler Sensores:** O Arduino faz a leitura principal do sensor Indutivo (CHE12-4N).
> 5.  **Tomar Decisão:**
>       * **SE** o sensor indutivo disparou (detectou metal):
>           * Mover o motor de passo para a **Posição B (Metal)**.
>       * **SENÃO** (não detectou metal):
>           * Manter (ou mover) o motor de passo na **Posição A (Não-Metal)**.
> 6.  **Resetar:** Após alguns segundos, mover o motor de volta para a posição padrão (Posição A) e voltar ao Passo 1, aguardando o próximo objeto.

## 🚀 Próximos Passos (Fase Experimental)

Uma vez que a separação de 2 categorias (Metal / Não-Metal) esteja funcionando perfeitamente, pode-se tentar usar o sensor de cor (TCS3200) para sub-dividir a categoria "Não-Metal".

  * **Exemplo de Lógica:**
      * `SE` (Indutivo == NÃO) E (Cor == Marrom):
          * Mover motor para Posição C (Talvez Papelão?).
      * `SE` (Indutivo == NÃO) E (Cor == Branco):
          * Mover motor para Posição D (Talvez Plástico/Papel Branco?).

**Atenção:** Esta fase é puramente experimental e terá uma alta taxa de falhas, dadas as limitações do sensor de cor.

## 📝 Licença

Este projeto está licenciado sob a Licença MIT.
