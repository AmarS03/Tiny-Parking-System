"use client";

import React, { useRef, Suspense, forwardRef, useImperativeHandle } from "react";
const Spline = React.lazy(() => import('@splinetool/react-spline'));

export function SplineLoader() {
    return (
        <div className="flex items-center justify-center h-full w-full text-gray-500 text-2xl">
            Loading Spline preview, wait...
        </div>
    );
}

export interface SplineComponentHandle {
    triggerEntryAnimation: (carNum: number) => void;
    triggerExitAnimation: (carNum: number) => void;
}

const SplineComponent = forwardRef<SplineComponentHandle>((props, ref) => {
    const splineContainerRef = useRef<HTMLDivElement>(null);
    const spline = useRef<any>(null);

    function handleLoad(app: any) {
        spline.current = app;
        console.log("Spline: instance ready");
    }

    function triggerEntryAnimation(carNum: number) {
        console.log(`Triggering entry animation for car ${carNum}`);
        spline.current?.emitEvent('keyUp', `Car ${carNum}`);
    }

    function triggerExitAnimation(carNum: number) {
        console.log(`Triggering exit animation for car ${carNum}`);
        spline.current?.emitEvent('keyDown', `Car ${carNum}`);
    }

    // Expose functions via ref
    useImperativeHandle(ref, () => ({
        triggerEntryAnimation,
        triggerExitAnimation,
    }));

    return (
        <div ref={splineContainerRef} className="h-full">
            <Suspense fallback={<SplineLoader />}>
                <Spline 
                    scene="https://prod.spline.design/vNKyquF2eCzlGbfm/scene.splinecode" 
                    onLoad={handleLoad}
                />
            </Suspense>
        </div>
    );
});

SplineComponent.displayName = 'SplineComponent';

export default SplineComponent;