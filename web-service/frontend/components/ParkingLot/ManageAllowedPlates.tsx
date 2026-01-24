'use client';

import { useState } from 'react';

import { Button } from '@/components/ui/button';
import { Input } from '@/components/ui/input';
import { Field, FieldLabel } from '@/components/ui/field';
import {
    Dialog,
    DialogContent,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogTrigger,
} from '@/components/ui/dialog';

import { Trash2, Settings2Icon, Plus, Badge } from 'lucide-react';
import { ButtonGroup } from '../ui/button-group';

// License plate regex pattern
const PLATE_REGEX = /^[A-Z0-9]{7}$/i;

export default function ManageAllowedPlates(
    {
        allowedPlates
    } : {
        allowedPlates: string[]
    }
) {
    const [plates, setPlates] = useState<string[]>(allowedPlates || []);
    const [open, setOpen] = useState(false);
    const [updating, setUpdating] = useState(false);
    const [newPlate, setNewPlate] = useState('');
    const [error, setError] = useState('');

    const handleDeletePlate = (plate: string) => {
        setPlates(plates.filter(p => p !== plate));
    };

    const handleAddPlate = () => {
        const trimmedPlate = newPlate.trim().toUpperCase();
        
        setError('');

        // Validation
        if (!trimmedPlate) {
            setError('Please enter a plate number');
            return;
        }

        if (!PLATE_REGEX.test(trimmedPlate)) {
            setError('Invalid plate format: use 7 alphanumeric characters');
            return;
        }

        if (plates.includes(trimmedPlate)) {
            setError('This plate is already in the list');
            return;
        }

        // Add plate to list
        setPlates([...plates, trimmedPlate]);
        setNewPlate('');
    };

    const handleSaveChanges = async () => {
        setUpdating(true);

        try {
            // Send the updated status back
            const putResponse = await fetch('http://localhost:5000/allowed', {
                method: 'PUT',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ allowedPlates: plates }),
            });

            if (!putResponse.ok) {
                throw new Error('Failed to update allowed plates');
            } else {
                setOpen(false);
                window.location.reload();
            }
        } catch (err) {
            setError(err instanceof Error ? err.message : 'Failed to save changes');
        }

        setUpdating(false);
    };

    return (
        <Dialog open={open} onOpenChange={setOpen}>
            <DialogTrigger asChild>
                <Button variant="outline" className="gap-2">
                    <Settings2Icon className="h-4 w-4" />
                    Manage allowed plates
                </Button>
            </DialogTrigger>
            <DialogContent className="max-w-md">
                <DialogHeader>
                    <DialogTitle>
                        Manage allowed plates
                    </DialogTitle>
                </DialogHeader>
                <div className="space-y-4">
                    {/* Add new plate form */}
                    <div className="space-y-2">
                        <div className="flex gap-2">
                            <Field>
                                <FieldLabel htmlFor="new-plate">
                                    Add new plate
                                </FieldLabel>
                                <ButtonGroup>
                                <Input 
                                    id="new-plate"
                                    type="text"
                                    placeholder="EW902DZ"
                                    value={newPlate}
                                    onChange={(e) => {
                                        setNewPlate(e.target.value);
                                        setError('');
                                    }}
                                />
                                <Button
                                    variant={"outline"}
                                    className="gap-1"
                                    onClick={handleAddPlate}
                                >
                                    <Plus className="h-4 w-4" />
                                    Add
                                </Button>
                                </ButtonGroup>
                            </Field>
                        </div>
                        {error && (
                            <p className="text-xs text-red-500">{error}</p>
                        )}
                    </div>
                </div>
                <div className="space-y-2">
                    {
                        plates.length === 0 ? (
                            <p className="text-sm text-gray-500">
                                No allowed plates configured yet
                            </p>
                        ) : (
                            plates.map((plate) => (
                                <div
                                    key={plate}
                                    className="flex items-center justify-between p-3 bg-gray-50 dark:bg-gray-900 rounded-lg border border-gray-200 dark:border-gray-700"
                                >
                                    <span className="font-mono font-semibold text-gray-900 dark:text-gray-100">
                                        {plate}
                                    </span>
                                    <button
                                        onClick={() => handleDeletePlate(plate)}
                                        className="p-2 text-red-500 hover:bg-red-50 dark:hover:bg-red-900/20 rounded transition-colors"
                                        aria-label={`Delete plate ${plate}`}
                                    >
                                        <Trash2 className="h-4 w-4" />
                                    </button>
                                </div>
                            ))
                        )
                    }
                </div>
                <DialogFooter>
                    <Button type="submit" onClick={handleSaveChanges} disabled={updating}>
                        {updating ? 'Saving...' : 'Save changes'}
                    </Button>
                </DialogFooter>
            </DialogContent>
        </Dialog>
    );
}