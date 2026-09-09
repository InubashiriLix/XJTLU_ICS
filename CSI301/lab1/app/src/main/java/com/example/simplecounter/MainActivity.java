package com.example.simplecounter;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends Activity {
    private static final String STATE_COUNT = "count";
    private int count = 0;
    private TextView countText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Keep the content clear of system bars on newer Android versions.
        View root = findViewById(R.id.main);
        root.setOnApplyWindowInsetsListener((view, insets) -> {
            view.setPadding(insets.getSystemWindowInsetLeft(),
                    insets.getSystemWindowInsetTop(),
                    insets.getSystemWindowInsetRight(),
                    insets.getSystemWindowInsetBottom());
            return insets;
        });
        root.requestApplyInsets();

        countText = findViewById(R.id.count_text);
        if (savedInstanceState != null) {
            count = savedInstanceState.getInt(STATE_COUNT, 0);
        }
        updateCount();

        findViewById(R.id.add_button).setOnClickListener(view -> {
            if (count < Integer.MAX_VALUE) {
                count++;
            }
            updateCount();
        });

        findViewById(R.id.reset_button).setOnClickListener(view -> {
            count = 0;
            updateCount();
        });
    }

    private void updateCount() {
        countText.setText(String.valueOf(count));
    }

    // Preserve the number when Android recreates the screen (for example, rotation).
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        outState.putInt(STATE_COUNT, count);
        super.onSaveInstanceState(outState);
    }
}
