Matheus Venturyne Xavier Ferreira - 21197 - 15/05/2015

Esse programa tem como objetivo resolver a eliminação de Gauss utilizando a biblioteca OpenMPI. O sistema consiste em duas classes principais: ServerNode e ClientNode. As duas classes herdam a classe Node e tem como objetivo resolver a eliminação de Gauss em um processo servidor junto a um conjunto de processos cliente. Alem disso, há a classe Matrix responsável por gerenciar a estrutura de dados da matriz. Para teste foram criados um conjunto de funções no arquivo TestFramework.

Como Utilizar:
O software consiste em dois executáveis que podem ser compilados executando o Makefile gerando 2 executáveis code (usado para rodar o programa) e testgen, que permite gerar casos de teste. O código apenas suporta matrizes que possuem número de linhas menor ou igual ao número de colunas.

Uma forma automática de execução consiste em adicionar as matrizes no arquivo test.txt e executar o script test.sh. O arquivo de testes possui vários casos de teste. Na primeira linha de um caso de teste há dois inteiros n (número de linhas), m (número de colunas) : n >= m. Cada uma das n linhas seguintes possuem m números reais. O último caso de teste é indicado por n = m = 0.

Entrada:
3 6
2 -1 0 1 0 0
-1 2 -1 0 1 0
0 -1 2 0 0 1
3 4
0 2 1 -8
1 -2 -3 0
-1 1 2 3
3 4
1 -2 -6 12
2 4 12 -17
1 -4 -12 22
0 0

Saída:
Teste 1
1 0 0 0.75 0.5 0.25
0 1 0 0.5 1 0.5
0 0 1 0.25 0.5 0.75

Teste 2
1 0 0 -4
0 1 0 -5
0 0 1 2

Teste 3
Matrix is singular!
Parallel Result:
1 0 0 1.66667
0 1 3 -5.08333
0 0 0 0.166666

Arquitetura:
-Matrix.h
Armazena uma matriz com n linhas e m colunas. Os elementos da matriz podem ser acessados utilizando o operador []. A classe implementa o método Serialize para converter a matriz em bytes, utilizada para enviar a matriz para os clientes, e o método Deserialize para recuperar a matriz no nó cliente.

-Common.h
Define:
.O tipo de dado uint_8 usado para representar um byte.
.A flag DEBUG para ativar ou desativar mensagens de logging.
.A constante PRECISION define a precisão para se considerar um ponto flutuante como nulo.

-TestFramework.h
.A função GaussElimination recebe uma matriz e calcula a eliminação de gauss 'in place' utilizando um algoritmo sequencial.
.A função GerenateTest recebe o número de casos de testes para serem criados.

-Node.h
Possui os atributos:

rank_: armazena o rank do processo. O rank é utilizado como uma forma para qualquer processo identificar quais as linhas são de sua responsabilidade.
num_processes_: armazena o número de processos executando
matrix_: a matriz a ser calculada
k_min_: primeira linha de responsabilidade desse processo.
k_max_: (k_max_ - 1) é a última linha de responsabilidade desse processo.

Métodos:
Init(): uma vez recebida a matriz, esse método inicializa o objeto.
int RankOfOwner(int k): recebe o número k de uma linha e retorna o rank do processo responsável por k.
Tournment(int k): realiza um torneio de todas as linhas >= k e retorna o número da linha que possui o maior valor absoluto na coluna k.
SwapRows(int dst, int src): troca as linhas dst e src na Matriz.
RequestPivot(int k): obtém a linha k da matriz. Utilizado por todos os processos para sincronizar a linha pivô.
Compute(k): computa a eliminação de Gauss para a linha k.
Join(): sincroniza a matriz no Servidor.

-ClientNode.h
Implementa os métodos:
Run(): chamada de execução do código do Cliente.
ReceiveBroadcastInput(): recebe a matriz do servidor.
GaussElimination(): computa a eliminação de Gauss do lado do Cliente

-ServerNode.h
Implementa os métodos:
Run(): chamada de execução do código do Servidor.
TestRun(): chamada de execução do código do Servidor para teste automático.
ReadInput(): leitura da matriz do stdin.
BroadcastInput(): broadcast da matriz para os clientes.
GaussElimination(): computa a eliminação de Gauss do lado do Servidor

Algoritmo:
	Deixe n ser o número de linhas
	Deixe m ser o número de colunas
	Deixe matrix[1...n][1...m] ser a matriz de entrada n x m
	For k = [1...n]
		Deixe pivô = ARG{i = [k...n] : max(m[i][k])} // Torneio entre os nós
		Troca a linha k com a linha pivô // O(s) dono(s) da linha k e pivô troca(m) sua(s) linha(s)
		Sincroniza a nova linha k entre todos os nós // O dono da linha k da um brodcast da linha
	
		If matrix[k][k] == 0 // A matriz é singular então paramos
			retorna m
		Endif
	
		Computa a eliminação de Gauss para a linha k // Cada nó possui a linha pivo logo calcula individualmente a eliminação de Gauss para suas linhas
	Endfor
	retorna m



	


