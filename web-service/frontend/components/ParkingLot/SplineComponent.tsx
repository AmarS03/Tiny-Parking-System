"use client";

import React, { useEffect, useRef, Suspense } from "react";
const Spline = React.lazy(() => import('@splinetool/react-spline'));

export function SplineLoader() {
    return (
        <div className="flex items-center justify-center h-full w-full text-gray-500 text-2xl">
            Loading Spline preview, wait...
        </div>
    );
}

export default function SplineComponent() {
    const splineContainerRef = useRef<HTMLDivElement>(null);

    function onSplineKeyDown(e: any) {
        console.log('Spline key event:', e.target?.name);
    }

    return (
        <div ref={splineContainerRef} className="h-full">
            <Suspense fallback={<SplineLoader />}>
                <Spline 
                    scene="https://prod.spline.design/dSDrlBsl8wQPTGyl/scene.splinecode" 
                    onSplineKeyDown={onSplineKeyDown} 
                />
            </Suspense>
        </div>
    );
}