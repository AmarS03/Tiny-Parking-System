export function isLicensePlateValid(licensePlate) {
	const plateRegex = /^[A-Z0-9]{1,7}$/;
	return !plateRegex.test(licensePlate);
}