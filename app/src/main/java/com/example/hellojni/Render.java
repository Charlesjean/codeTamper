package com.example.hellojni;
import android.content.Context;
import android.graphics.Color;
import android.view.View;
import android.widget.Toast;

/**
 * Created by duanjin on 4/4/15.
 */
public class Render {
    private Context mCtx;
    public Render(Context ctx) {
        mCtx = ctx;
    }
    public void paintToRed(View target) {
        target.setBackgroundColor(Color.RED);
        Toast.makeText(mCtx, "paintToRed invoked", Toast.LENGTH_SHORT).show();
    }
    public void paintToGreen(View target) {
        target.setBackgroundColor(Color.GREEN);
    }
    public void paintToBlue(View target) {
        target.setBackgroundColor(Color.BLUE);
        Toast.makeText(mCtx, "paintToBlue invoked", Toast.LENGTH_SHORT).show();
    }
}
