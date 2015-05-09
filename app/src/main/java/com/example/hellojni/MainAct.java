/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.hellojni;
import android.app.Activity;
import android.view.View;
import android.widget.TextView;
import android.os.Bundle;

/**
 * Created by duanjin on 4/4/15.
 */
public class MainAct extends Activity
{
    /** Called when the activity is first created. */
    View mColorPanel;
    Render mRender;
    static {
        System.loadLibrary("dexfileparser");
    }
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        mRender = new Render(this);
        setContentView(R.layout.main_ly);
        mColorPanel = findViewById(R.id.panel);
        TextView redColor = (TextView) findViewById(R.id.red);
        TextView blueColor = (TextView) findViewById(R.id.green);
        redColor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mRender.paintToRed(mColorPanel);

            }
        });
        blueColor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mRender.paintToBlue(mColorPanel);
            }
        });
    }
}
