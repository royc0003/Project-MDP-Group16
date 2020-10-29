package com.example.mdpgroup16;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;

public class PageAdapter extends FragmentPagerAdapter {

    private int numberoftabs;
    public Fragment fragment1, fragment2, fragment3;

    public PageAdapter(@NonNull FragmentManager fm, int numberoftabs) {
        super(fm);
        fragment1 = new MapTab();
        fragment2 = new BluetoothTab();
        fragment3 = new CommsTab();
        this.numberoftabs = numberoftabs;
    }

    @NonNull
    @Override
    public Fragment getItem(int position) {
        switch(position){
            case 0:
                return fragment1;
            case 1:
                return fragment2;
            case 2:
                return fragment3;
            default:
                return null;
        }
    }

    @Override
    public int getCount() {
        return numberoftabs;
    }

    @Override
    public int getItemPosition(@NonNull Object object) {
        return POSITION_NONE;
    }
}
