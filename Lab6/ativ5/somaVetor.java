class Adder implements Runnable {

	private int[] vetor;

	private int start;
	private int end;

	public Adder(int[] vetor, int start, int end) {
		this.vetor = vetor;
		this.start = start;
		this.end = end;
	}

	public void run() {
		for (int i = start; i < end; i++) {
            this.vetor[i] += 1;
		}
	}
}

class somaVetor {
	private static int N = 2;
	private static int ARR_LEN = 10;

	public static void main(String args[]) {
		Thread[] threads = new Thread[N];
		
        // Inicializa o vetor
		int[] vetor = new int[ARR_LEN];
		for (int i = 0; i < vetor.length; i++) {
			vetor[i] = i;
		}
		
		int offset = 0;
		int len = vetor.length / N;
		int resto = vetor.length % N;

		// Cria as threads da aplicacao
		for (int i = 0; i < threads.length; i++) {
			int start = offset;
			int end = start + len;

			offset += len;
			if ( resto > 0 ) {
				end += 1;
				offset += 1;
				resto -= 1;
			}

			threads[i] = new Thread(new Adder(vetor, start, end));
		}

		// Inicia as threads
		for (int i = 0; i < threads.length; i++) {
			threads[i].start();
		}

		// Espera pelo termino de todas as threads
		for (int i = 0; i < threads.length; i++) {
			try { threads[i].join(); }
            catch (InterruptedException e) { return; }
		}

        // Checar se está certo
        int errs = 0;
        for (int i = 0; i < vetor.length; i++) {
            if ( vetor[i] != i+1 ) {
                errs += 1;
                System.out.printf("vetor[%d] = %d; esperado = %d", 
                        i, vetor[i], i+1);
            }
        }
        if ( errs > 0 ) {
            System.out.println("Total de erros: " + errs);
        } else {
            System.out.println("Tudo ok!");
        }
	}
}
