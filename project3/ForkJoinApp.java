import java.util.concurrent.*;

public class ForkJoinApp{
    private static final int SIZE = 20;


	public static void main(String[] args){
		ForkJoinPool pool = new ForkJoinPool();
        int[] array = new int[SIZE];
        int[] array1 = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
            array[i] = rand.nextInt(SIZE);
            array1[i] = rand.nextInt(SIZE);            
		}		
		
        // use fork-join parallelism to sum the array


        // quick
        System.out.print("Origin Array 1: ");
        for (int i=0; i<SIZE; i++){
            System.out.printf("%d ", array[i]);
        }
        System.out.print("\n");
		QuickSort quickSort = new QuickSort(array, 0, SIZE);
        pool.invoke(quickSort);
        
        System.out.print("Sorted Array 1: ");
        for (int i=0; i<SIZE; i++){
            System.out.printf("%d ", array[i]);
        }
        System.out.print("\n");
        
        // Merge
        System.out.print("Origin Array 2: ");
        for (int i=0; i<SIZE; i++){
            System.out.printf("%d ", array1[i]);
        }
        System.out.print("\n");

        MergeSort mergeSort = new MergeSort(array1, 0, SIZE);

        pool.invoke(mergeSort);

        System.out.print("Sorted Array 2: ");
        for (int i=0; i<SIZE; i++){
            System.out.printf("%d ", array1[i]);
        }
        System.out.print("\n");

    }
}


class QuickSort extends RecursiveAction{

    /**
     *
     */
    private static final long serialVersionUID = 1L;

    static final int THRESHOLD = 5;

    private int array[];
    private int low;
    private int high;

    public QuickSort(int[] array, int low, int high){
        this.array = array;
        this.low = low;
        this.high = high;
    }

    @Override
    protected void compute(){
        if(low < high - THRESHOLD){
            int mr = array[low];
            int pos = low;
            for(int i=low+1;i<high;i++){
                if(array[i]<mr){
                    int tmp = array[i];
                    array[i] = mr;
                    array[pos++] = tmp;
                }
            }
            
            QuickSort leftSort = new QuickSort(array, low, pos);
            QuickSort rightSort = new QuickSort(array, pos+1, high);
            invokeAll(leftSort,rightSort);
            
        }else{
            bubbleSort();
        }
    }

    private void bubbleSort(){
        for(int i=low;i<high;i++){
            for(int j=i+1;j<high;j++){
                if(array[i]>array[j]){
                    int tmp = array[j];
                    array[j] = array[i];
                    array[i] = tmp;
                }
            }
        }
    }
    
}

class MergeSort extends RecursiveAction{

    /**
     *
     */
    private static final long serialVersionUID = 1L;

    static final int THRESHOLD = 5;

    private int array[];
    private int low;
    private int high;

    public MergeSort(int[] array, int low, int high){
        this.array = array;
        this.low = low;
        this.high = high;
    }

    @Override
    protected void compute(){
        if(this.low < this.high - THRESHOLD){
            int mid = (low+high)/2;
            MergeSort leftSort = new MergeSort(this.array, this.low, mid);
            MergeSort rightSort = new MergeSort(this.array, mid, this.high);
            invokeAll(leftSort,rightSort);
            merge(this.low,this.high,mid);
        }else{
            bubbleSort();
        }

    }

    
    private void bubbleSort(){
        for(int i=this.low;i<this.high;i++){
            for(int j=i+1;j<this.high;j++){
                if(this.array[i]>this.array[j]){
                    int tmp = this.array[j];
                    this.array[j] = this.array[i];
                    this.array[i] = tmp;
                }
            }
        }
    }


    /**
     * 
     * @param low
     * @param high
     * @param mid
     */
    private void merge(int l, int h, int m){
        int res[] = new int[high-low];
        int left = l;
        int right = m;
        int pos = 0;
        while(left<m && right<h){
            if(this.array[left]<this.array[right]){
                res[pos++]=this.array[left++];
            }else{
                res[pos++]=this.array[right++];
            }
        }
        while(left<m){
            res[pos++]=this.array[left++];
        }
        while(right<h){
            res[pos++]=this.array[right++];
        }

        for(int i=0;i<h-l;i++){
            this.array[l+i] = res[i];
        }
    }
}