
<br />
<p align="center">

  <h3 align="center">Monitor em Java</h3>

  <p align="center">
    Modulo 2 - Laboratório 7
    <br />
   Computação Concorrente
   <br />
   <a href="/Verificação em Haskell (Versão Original)">Programa com validação em Haskell (Versão original)</a>
   ·
   <a href="Verificação em Python">Programa com validação em Python</a>
  </p>
</p>


<details open="open">
  <summary>Sumário</summary>
  <ol>
  <li>
	  <a href="#laboratório">Laboratório</a>
  </li>
    <li>
        <a href="#validações">Validações</a>
        <ul>
            <li><a href="#haskell">Haskell</a></li>
            <li><a href="#python">Python</a></li>
      </ul>
    </li>
  </ol>
</details>

## Laboratório

O objetivo deste lab é projetar e testar um programa concorrente simulando o conceito de monitor em Java para produtores e consumidores de um buffer. A descrição completa deste lab poderá ser encontrada [neste link](lab7.pdf).


## Validações

### Haskell

Submissão principal do laboratório

#### Passo 1

Para executar a validação do programa concorrente utilizando o parser feito em Haskell basta ter o [ghc instalado](https://www.haskell.org/platform/) e compilar os arquivos com o seguinte comando

```sh
    ghc -o haskell haskell.hs
  ```


Com o objetivo de facilitar caso o ambiente não esteja preparado para essa compilação, está sendo disponibilizado no diretório <a href="/Verificação em Haskell (Versão Original)">Validação em Haskell (Versão original)</a> os programas já compilados, para isso poderá ser executado o programa diretamente sem o processo anterior de compilação.

#### Passo 2

Com o arquivo do parser do haskell disponível, será necessário executar o nosso programa concorrente em Java redirecionando para um arquivo de saída

```sh
    java Main > in
  ```

OBS.: Lembrando que os arquivos também deverão ser compilados
```sh
    javac buffer.java
    javac main.java
  ```
mas todos os arquivos já estarão disponíveis no diretório.

Como o programa executa em loop, para a execução no momento desejado com `ctrl+c`

#### Passo 3

Para executar a validação, será necessário rodar o validador passando o arquivo gerado na etapa anterior. Para isso execute

```sh
    ./haskell < in
  ```

Caso não seja exibida nenhuma mensagem com erro no terminal e aparecer somente "Tudo ok!", a execução ocorreu conforme o esperado.


### Python

Submissão secundária do laboratório

#### Passo 1

Para executar a validação do programa concorrente utilizando o parser feito em Python basta ter o [python3 instalado](https://www.python.org/downloads/)

#### Passo 2

Agora será necessário executar o nosso programa concorrente em Java redirecionando para um arquivo de saída

```sh
    java Main > validacao.py
  ```

OBS.: Lembrando que os arquivos também deverão ser compilados

```sh
    javac buffer.java
    javac main.java
  ```

mas todos os arquivos já estarão disponíveis no diretório.

Como o programa executa em loop, para a execução no momento desejado com `ctrl+c`

#### Passo 3

Para executar a validação, será necessário rodar o validador passando o arquivo gerado na etapa anterior. Para isso execute

```sh
    python validacao.py
  ```

Caso não seja exibida nenhuma mensagem com erro no terminal a execução ocorreu conforme o esperado.
