import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.*;

import java.io.IOException;
import java.net.URI;
import java.util.Date;

public class test_hopsfs {
    public static void main(String[] args) throws IOException {
        Configuration conf = new Configuration();
        FileSystem fs = FileSystem.get(conf);

        Date now = new Date();
        long st = now.getTime(); //ms

        for (int i = 0; i < 10000; i++)
        {
            fs.mkdirs(new Path("/test/" + i));
        }
        
        now = new Date();
        long ed = now.getTime();

        long latency= ed - st;
        System.out.println(latency);  
        fs.close();
    }
}

// Result: 
//     c jin test mkdir average latency is 2.7219 ms
//     java test mkdir average latency is 2.7540 ms 
//     jni is negligible 